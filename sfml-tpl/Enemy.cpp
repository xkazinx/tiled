#include "stdafx.h"
#include "Enemy.h"
#include "Camera.h"
#include "SGame.h"
#include "Player.h"
#include "PDot.h"

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
	if (true)
	{
		if (_fire_timer.getElapsedTime().asMilliseconds() >= 1000)
		{
			_fire_timer.restart();
			auto& pos = getPosition();
			
			g_game->_projectiles.push_back(mk<PDot>(
				getPosition(), g_player->getPosition()
				));
		}
	}

	auto velocity = 0.05f;
	move(0.05f - velocity * delta.asMilliseconds(), 0.f);
}

Object::Collider Enemy::GetCollider() const
{
	auto & pos = getPosition();
	auto rect = _shape.getGlobalBounds();
	return sf::FloatRect(rect.left + pos.x, rect.top + pos.y, rect.width, rect.height);
}

bool Enemy::_HasExpired() const {
	return getPosition().x < g_camera->GetLeft();
}

void Enemy::HandleCollition(const Object * object)
{
	if (object->Is(ObjectID::PNormal)) {
		Expire();
	}
}

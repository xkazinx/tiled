#include "stdafx.h"
#include "Player.h"
#include "Camera.h"
#include "SGame.h"
#include "SceneMgr.h"
#include "PNormal.h"

Player::Player(const Vector2f & pos) 
	: Object(pos, ObjectID::Player),
	_triangle(20.f, 3),
	_weapon(Weapon::Normal)
{
	_triangle.setOrigin({ _triangle.getRadius(), _triangle.getRadius() });
	_triangle.rotate(90.f);

	_circles[0].setFillColor(sf::Color::Red);
	_circles[0].setRadius(10.f);
	_circles[0].setPosition({ -18.f, -25.f });
	
	_circles[1].setFillColor(sf::Color::Red);
	_circles[1].setRadius(10.f);
	_circles[1].setPosition({ -18.f, 5.f });
}

Player::~Player()
{
}

void Player::Render(TargetStatesRef && ts) const
{
	if (_weapon != Weapon::Normal)
	{
		ts().draw(_circles[0], ts.states);
		ts().draw(_circles[1], ts.states);
	}

	ts().draw(_triangle, ts.states);
}

void Player::Update(const sf::Time & delta)
{
	static sf::Clock timer;
	if (Key::isKeyPressed(Key::A))
	{
		if (timer.getElapsedTime().asMilliseconds() >= 100)
		{
			timer.restart();

			auto& pos = g_player->getPosition();
			switch (g_player->_weapon)
			{
			case Weapon::Normal:
				g_game->_projectiles.push_back(mk<PNormal>(
					g_player->getPosition()
					));
				break;

			case Weapon::Triple:
				for (u8 i = 0; i < 3; i++)
				{
					g_game->_projectiles.push_back(mk<PNormal>(
						sf::Vector2f{ pos.x, pos.y - 15.f + (i * 15.f) })
					);
				}
				break;
			}
		}
	}

	constexpr auto camera_velocity = .05f;
	Vector2f velocity(camera_velocity, 0.f);

	// Idle velocity
	velocity.x *= -1.f / 3.f;

	// velocity.x
	if (Key::isKeyPressed(Key::Right) ||
		Key::isKeyPressed(Key::Left))
	{
		auto direction = Key::isKeyPressed(Key::Left) ? -1 : 1;
		velocity.x += 5 * camera_velocity * direction;
	}

	// velocity.y
	if (Key::isKeyPressed(Key::Up) ||
		Key::isKeyPressed(Key::Down))
	{
		auto direction = Key::isKeyPressed(Key::Up) ? -1 : 1;
		velocity.y += 0.3f * direction;
	}

	static sf::FloatRect bounds;

	// Player bounds
	bounds.left = 50.f + g_camera->GetLeft();
	bounds.top = g_camera->GetTop();
	bounds.width = g_camera->getSize().x - 0.2f * g_camera->getSize().x;
	bounds.height = g_camera->getSize().y;

	const auto & current_pos = getPosition();
	move(DeltaVelocity(velocity, delta));
	if (!bounds.contains(getPosition()))
	{
		auto pos = current_pos;

		if (pos.x < bounds.left) {
			pos.x = bounds.left + 1.f;
			pos.y = getPosition().y;
		}

		if (pos.y < bounds.top || pos.y > bounds.top + bounds.height)
		{
			pos.x = getPosition().x;
			pos.y = pos.y < bounds.top ? bounds.top + 1.f : bounds.top + bounds.height - 1.f;
		}

		setPosition(pos);
	}
}

void Player::SetWeapon(Weapon weapon)
{
	_weapon = weapon;
	switch (weapon)
	{
	case Weapon::Normal:
		
		break;

	case Weapon::Laser:
		for (auto& circle : _circles)
		{
			circle.setFillColor(sf::Color::Green);
		}
		break;

	case Weapon::Triple:
		for (auto& circle : _circles)
		{
			circle.setFillColor(sf::Color::Blue);
		}
		break;
	}
}

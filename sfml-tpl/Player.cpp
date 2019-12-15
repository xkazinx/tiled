#include "stdafx.h"
#include "Player.h"
#include "Camera.h"

Player::Player(const Vector2f & pos) 
	: Object(pos, ObjectID::Player),
	_triangle(20.f, 3)
{
	_triangle.setOrigin({ _triangle.getRadius(), _triangle.getRadius() });
	_triangle.rotate(90.f);
}

Player::~Player()
{
}

void Player::Update(const sf::Time & delta)
{
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

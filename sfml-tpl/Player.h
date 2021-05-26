#pragma once
#include "Object.h"

class Player : public Object
{
public:
	Player(const Vector2f & pos = { 0.f, 0.f });

	~Player();
	
	void Render(TargetStatesRef && ts) const override;

	sf::CircleShape _triangle, _circles[2];
	sf::Vector2f _velocity;

	void Update(const sf::Time & delta) override;
	void SetWeapon(Weapon weapon);
	Weapon _weapon;
};


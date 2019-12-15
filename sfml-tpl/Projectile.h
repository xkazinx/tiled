#pragma once
#include "Object.h"

class Projectile : public Object
{
public:
	Projectile(const Vector2f & pos = { 0.f, 0.f });
	~Projectile();

	void Render(TargetStatesRef && ts) const override;
	void Update(const sf::Time & delta) override;

	sf::RectangleShape _shape;
	Vector2f _initial_pos;
		
	Collider GetCollider() const override {
		auto & pos = getPosition();
		auto rect = _shape.getGlobalBounds();
		return sf::FloatRect(rect.left + pos.x, rect.top + pos.y, rect.width, rect.height);
	}

	void HandleCollition(const Object * object) override;

private:
	bool _HasExpired() const override;
};


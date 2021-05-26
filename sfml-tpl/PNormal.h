#pragma once
#include "Object.h"

class PNormal : public Object
{
public:
	PNormal(const Vector2f& pos);

	void Render(TargetStatesRef&& ts) const override;
	void Update(const sf::Time& delta) override;

	sf::RectangleShape _shape;

	Collider GetCollider() const override;

	void HandleCollition(const Object* object) override;
private:
	bool _HasExpired() const override;
	Vector2f _initial_pos;
};


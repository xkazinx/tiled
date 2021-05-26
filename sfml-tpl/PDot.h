#pragma once
#include "Object.h"

class PDot : public Object
{
public:
	PDot(const Vector2f& pos, const Vector2f& end_pos);

	void Render(TargetStatesRef&& ts) const override;
	void Update(const sf::Time& delta) override;

	sf::CircleShape _shape;

	Collider GetCollider() const override;

	void HandleCollition(const Object* object) override;
private:
	bool _HasExpired() const override;
	Vector2f _initial_pos, _end_pos;
};


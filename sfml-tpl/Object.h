#pragma once

class Object : public sf::Drawable, public sf::Transformable
{
private:
	bool _has_expired = false;
	virtual bool _HasExpired() const { return false; }

	void _DebugRender(TargetStatesRef && ts) const;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override final
	{
		states.transform *= getTransform();
		Render({ target, states });

#ifdef DEBUG_MODE
		states.transform *= getInverseTransform();
		_DebugRender({ target, states });
#endif
	}

protected:
	void Expire() { _has_expired = true; }

public:
	Object(const Vector2f & pos = { 0.f, 0.f }, const ObjectID & id = ObjectID::None)
		: _id(id) 
	{
		setPosition(pos);
	}

	virtual ~Object();

	virtual void Render(TargetStatesRef && ts) const = 0;

	virtual void Update(const sf::Time & delta) = 0;

	virtual bool HasExpired() const { return _HasExpired() || _has_expired; }

	virtual bool CollidesWith(const Object * object) const {

		if (!GetCollider().has_value() || !object->GetCollider().has_value())
		{
			return false;
		}

		return GetCollider().value().intersects(object->GetCollider().value());	
	}

	virtual void HandleCollition(const Object * object) {}

	using Collider = std::optional<sf::FloatRect>;
	virtual Collider GetCollider() const {
		return std::nullopt;
	}

	const ObjectID _id;
	auto Is(const ObjectID & id) const { return _id == id; }

	template<typename T>
	T * Get()
	{
		if constexpr (std::is_base_of_v<Object, T>) {
			return dynamic_cast<T>(this);
		}

		static_assert("Class not base of Object");
		return this;
	}

	
};
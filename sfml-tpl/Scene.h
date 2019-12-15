#pragma once
#include "Object.h"

class UIComponent;
class Scene : public sf::Drawable
{
private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
	using ID = scene::ID;

	Scene(const ID & id);
	~Scene();

	virtual void Render(TargetStatesRef && ts) const = 0;

	virtual void Update(const sf::Time & delta) = 0;

	virtual void Reset() = 0;

	void HandleEvents(const sf::Event & event);

	virtual void HandleEvent(const sf::Event & event) = 0;

	ID _id;

	vector<UIComponent*> _ui_components;

	template<typename T>
	unique_ptr<T> AddComponent(unique_ptr<T> && component)
	{
		_ui_components.push_back(component.get());
		return std::move(component);
	}
};
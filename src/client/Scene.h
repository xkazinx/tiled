#pragma once

class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	virtual void Tick() = 0;
	virtual void Render(sf::RenderTarget& tgt, const sf::RenderStates& states) const = 0;
	virtual void HandleEvents(const sf::Event& ev) = 0;
	virtual void ProcessUI() const {}
};
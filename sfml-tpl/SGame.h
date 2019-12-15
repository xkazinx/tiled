#pragma once
#include "Scene.h"

class SGame : public Scene
{
public:
	SGame();
	~SGame();

	void Reset() override;
	void Update(const sf::Time & delta) override;
	void Render(TargetStatesRef && ts) const override;
	void HandleEvent(const sf::Event & event) override;

	unique_ptr<Background> _background;

	auto & GetBackground() {
		return _objects[0];
	}

	Player * GetPlayer();

	vector<unique_ptr<Object>> _objects;
};

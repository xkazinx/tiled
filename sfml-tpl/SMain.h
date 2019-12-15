#pragma once
#include "Scene.h"

class SMain : public Scene
{
public:
	SMain();
	~SMain();

	void Reset() override;
	void Update(const sf::Time & delta) override;
	void Render(TargetStatesRef && ts) const override;
	void HandleEvent(const sf::Event & event) override;

	unique_ptr<Button> _button;
	unique_ptr<Background> _background;
};


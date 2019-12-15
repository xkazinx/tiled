#pragma once

class UIComponent : public sf::Drawable
{
public:
	UIComponent();
	~UIComponent();

	virtual bool HandleEvent(const sf::Event & event) = 0;
};


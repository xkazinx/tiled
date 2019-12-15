#pragma once
#include "UIComponent.h"

class Button : public UIComponent
{
public:
	using EventFunction = std::function<void(const sf::Event & event)>;

	auto & operator->() {
		return _text;
	}

	Button() {}
	Button(EventFunction && func, const sf::Vector2f & pos = { 0.f, 0.f }, const string & str = "");
	~Button();

private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		const float x_padding = 20.f;

		sf::RectangleShape rect({ _rect.width + x_padding, _rect.height });

		auto pos = _text.getPosition();

		pos.x -= x_padding / 2.f;

		rect.setPosition(pos);
		rect.setFillColor(sf::Color(255, 255, 255, 80));
		rect.setOutlineColor(sf::Color::White);
		rect.setOutlineThickness(3.f);

		target.draw(rect, states);
		target.draw(_text, states);
	}

	bool HandleEvent(const sf::Event & event) override;

	sf::Text _text;
	sf::FloatRect _rect;

	EventFunction _click_func;
};


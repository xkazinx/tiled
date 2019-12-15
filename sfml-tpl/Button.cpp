#include "stdafx.h"
#include "Button.h"
#include "TextMgr.h"

Button::Button(EventFunction && click_func, const sf::Vector2f & pos, const string & str)
	: _click_func(click_func)
{
	_text.setPosition(pos);
	_text.setString(str);
	_text.setFont(g_text->GetFont(font::ID::Arial));
	_text.setFillColor(sf::Color::White);

	_rect = sf::FloatRect(
		pos.x,
		pos.y,
		_text.getLocalBounds().width,
		(float)_text.getCharacterSize() + _text.getLocalBounds().top
	);
}

Button::~Button()
{

}

bool Button::HandleEvent(const sf::Event & event)
{
	if (event.type == sf::Event::MouseButtonReleased &&
		_rect.contains({ (float) event.mouseButton.x, (float) event.mouseButton.y })) {
		_click_func(event);
		return false;
	}
	return true;
}

#include "stdafx.h"
#include "TextMgr.h"
#include "App.h"

TextMgr::TextMgr()
{
	// Load fonts from font ids
	for (auto i = 0; i < ncast(ID::Max); i++) {
		_fonts[i].loadFromFile(font::kNames[i]);
	}

	_text.setFont(GetFont(ID::Arial));

	_text.setCharacterSize(24); // in pixels, not points!
	_text.setFillColor(sf::Color::Red);
	//_text.setStyle(sf::Text::Bold | sf::Text::Underlined);
	
}

TextMgr::~TextMgr()
{
}

void TextMgr::DrawText(const string & text, const Vector2f & pos)
{
	_text.setString(text);
	_text.setPosition(pos);

	g_app->_window.draw(_text);
}

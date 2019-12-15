#pragma once

class TextMgr
{
public:
	using ID = font::ID;

	TextMgr();
	~TextMgr();

private:
	array<sf::Font, ncast(ID::Max)> _fonts;
	sf::Text _text;

public:
	auto & GetFont(const ID & id) { return _fonts[ncast(id)]; }
	auto & GetFont(const ID & id) const { return _fonts[ncast(id)]; }

	void DrawText(const string & text, const Vector2f & pos);
};


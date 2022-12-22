#include "stdafx.h"
#include "Global.h"
#include "Scenes.h"
#include "GameScene.h"
#include "Player.h"
#include "Map.h"
#include "App.h"
#include "Units.h"

namespace Client
{
namespace Global
{
	App * g_app;
	unique_ptr<Scenes> g_scene;
	unique_ptr<kazin::Socket>	g_sock;
	unique_ptr<Map> g_map;
	GameScene * g_game = nullptr;
	unique_ptr<Player> g_player;
	unique_ptr<Units> g_units;

	void Initialize()
	{
		// unordered
		g_scene	= make_unique<Scenes>();
		g_units  = make_unique<Units>();

		for(auto font_name : enum_names<font::ID>())
		{
			font::List.push_back({});

			string file_name = font_name.data();
			kazin::ToLower(file_name);
			if(!font::List.back().loadFromFile("..\\data\\" + file_name + ".ttf"))
				PrintError("Couldn't load font {}", file_name);
		}

		font::Text.setCharacterSize(10);
		font::Text.setFont(font::List.back());

		// ordered
	}

	void Deinitialize()
	{

	}

	void Send(const kazin::OutPktRef out, enet_uint8 channel)
{
		g_sock->Send(out, channel);
	}
}

namespace font
{
	FontNames kNames = 
	{
		"arial.ttf",
	};

	vector<sf::Font> List;

	sf::Text Text;

	void Internal(RenderPoint pt, const char* str, sf::Color color)
	{
		auto & text = Text;
		text.setPosition({ float(pt.x), float(pt.y) });
		text.setString(str);
		text.setFillColor(color);

		g_app->_window.draw(text);
	}

	void wrapText(string& str, unsigned width, const sf::Font& font, unsigned charicterSize, bool bold)
	{
		unsigned currentOffset = 0;
		bool firstWord = true;
		std::size_t wordBegining = 0;

		auto sz = str.size();
		for (std::size_t pos(0); pos < sz; ++pos) {
			auto currentChar = str[pos];
			if (currentChar == '\n') {
				currentOffset = 0;
				firstWord = true;
				continue;
			}
			else if (currentChar == ' ') {
				wordBegining = pos;
				firstWord = false;
			}

			auto glyph = font.getGlyph(currentChar, charicterSize, bold);
			currentOffset += (unsigned int)glyph.advance;

			if (!firstWord && currentOffset > width) {
				pos = wordBegining;
				str[pos] = '\n';
				firstWord = true;
				currentOffset = 0;
			}
		}
	}

	void InternalWrap(const RECT& rect, const char* str, sf::Color color)
	{
		auto & text = Text;
		string txt = str;

		wrapText(txt, rect.right,
			font::List.back(), text.getCharacterSize(), false);

		text.setString(txt);

		float width = text.getLocalBounds().width / 2;
		int x = (int)(rect.left + (rect.right / 2) - width);

		text.setPosition({ float(x), float(rect.top) });

		text.setFillColor(color);

		g_app->_window.draw(text);
	}

	void Fixed(const RenderPoint& pt, string str, sf::Color color)
	{
		auto ptr = str.c_str();
		Internal(pt, ptr, color);
	}


	void Aligned(RenderPoint pt, RenderPointValue wdt, string str, sf::Color color)
	{
		auto ptr = str.c_str();
		RECT rect =
		{
			pt.x, pt.y,
			wdt, pt.y + 15
		};

		InternalWrap(rect, ptr, color);
	}
}
}

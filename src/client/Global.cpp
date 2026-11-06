#include "stdafx.h"
#include "Global.h"
#include "Scenes.h"
#include "GameScene.h"
#include "Player.h"
#include "Map.h"
#include "App.h"
#include "Units.h"
#include <os/exe_path.h>

namespace Client
{
namespace Global
{
	App		 * g_app;
	GameScene * g_game = nullptr;

	unique_ptr<Scenes>			g_scene;
	unique_ptr<SocketHandler>	g_sock;
	unique_ptr<Map>				g_map;
	unique_ptr<Player>			g_player;
	unique_ptr<Units>			g_units;

	// The fonts' folder: deploy\data beside the exe's deploy\client (the web build:
	// /data, preloaded)
	static string DataDir()
	{
#ifdef __EMSCRIPTEN__
		return "/data/";
#else
		return (lib::exe::Dir() / ".." / "data").string() + "/";
#endif
	}

	void Initialize()
	{
		// unordered
		g_units = make_unique<Units>();

		for (auto font_name : enum_names<font::ID>())
		{
			font::List.push_back({});

			string file_name(font_name);
			std::transform(file_name.begin(), file_name.end(), file_name.begin(), [](const unsigned char c) { return (char)std::tolower(c); });
			if (!font::List.back().openFromFile(DataDir() + file_name + ".ttf"))
				Err("Couldn't load font {}", file_name);
		}

		font::Text = make_unique<sf::Text>(font::List.back());
		font::Text->setCharacterSize(10);

		// ordered: the first scene draws with the fonts
		g_scene = make_unique<Scenes>();
	}

	void Deinitialize()
	{

	}

	void Send(OPktPtr & out, enet_uint8 channel)
	{
		out->Send(g_sock->Get(), channel);
	}
}

namespace font
{
	FontNames kNames =
	{
		"arial.ttf",
	};

	vector<sf::Font> List;

	unique_ptr<sf::Text> Text;

	void Internal(RenderPoint pt, const char * str, sf::Color color)
	{
		auto & text = *Text;
		text.setPosition({ float(pt.x), float(pt.y) });
		text.setString(str);
		text.setFillColor(color);

		g_app->_window.draw(text);
	}

	void wrapText(string & str, unsigned width, const sf::Font & font, unsigned charicterSize, bool bold)
	{
		unsigned currentOffset = 0;
		bool firstWord = true;
		std::size_t wordBegining = 0;

		auto sz = str.size();
		for (std::size_t pos(0); pos < sz; ++pos) {
			auto currentChar = str[pos];
			if (currentChar == '\n')
			{
				currentOffset = 0;
				firstWord = true;
				continue;
			}
			else if (currentChar == ' ')
			{
				wordBegining = pos;
				firstWord = false;
			}

			auto glyph = font.getGlyph((unsigned char)currentChar, charicterSize, bold);
			currentOffset += (unsigned int)glyph.advance;

			if (!firstWord && currentOffset > width)
			{
				pos = wordBegining;
				str[pos] = '\n';
				firstWord = true;
				currentOffset = 0;
			}
		}
	}

	void InternalWrap(const Box & rect, const char * str, sf::Color color)
	{
		auto & text = *Text;
		string txt = str;

		wrapText(txt, rect.right,
			font::List.back(), text.getCharacterSize(), false);

		text.setString(txt);

		float width = text.getLocalBounds().size.x / 2;
		int x = (int)(rect.left + (rect.right / 2) - width);

		text.setPosition({ float(x), float(rect.top) });

		text.setFillColor(color);

		g_app->_window.draw(text);
	}

	void Fixed(const RenderPoint & pt, string str, sf::Color color)
	{
		auto ptr = str.c_str();
		Internal(pt, ptr, color);
	}

	void Aligned(RenderPoint pt, RenderPointValue wdt, string str, sf::Color color)
	{
		auto ptr = str.c_str();
		Box rect =
		{
			pt.x, pt.y,
			wdt, pt.y + 15
		};

		InternalWrap(rect, ptr, color);
	}
}
}

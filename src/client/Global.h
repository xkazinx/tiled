#pragma once

namespace Client
{
struct App;
struct Scenes;
struct Units;
struct Map;
struct GameScene;
struct Player;

namespace Global
{
	constexpr char kAppName[] = "Tiled";

	extern App			* g_app;
	extern GameScene	* g_game;

	extern unique_ptr<Scenes>			g_scene;
	extern unique_ptr<SocketHandler>	g_sock;
	extern unique_ptr<Map>				g_map;
	extern unique_ptr<Player>			g_player;
	extern unique_ptr<Units>			g_units;

	extern void Initialize();
	extern void Deinitialize();

	extern void Send(OPktPtr & out, enet_uint8 channel = 0);

	namespace scene
	{
		enum ID : u8
		{
			Login,
			Main,
			Lobby,
			Game,
			Result,
		};
	}
}

namespace font {
	enum class ID : u8 {
		Arial,
	};

	// A text's box: left, top, its width (right) and bottom
	struct Box
	{
		int left = 0, top = 0, right = 0, bottom = 0;
	};

	using FontNames = const vector<string>;
	extern FontNames kNames;

	// The text every font:: call draws with (made once the fonts loaded)
	extern unique_ptr<sf::Text> Text;

	extern vector<sf::Font> List;

	extern void Internal(RenderPoint pt, const char * str, sf::Color color = sf::Color::White);
	extern void wrapText(string & str, unsigned width, const sf::Font & font, unsigned charicterSize, bool bold = false);
	extern void InternalWrap(const Box & rect, const char * str, sf::Color color);
	extern void Fixed(const RenderPoint & pt, string str, sf::Color color);
	extern void Aligned(RenderPoint pt, RenderPointValue wdt, string str, sf::Color color);
}

using SceneID = Global::scene::ID;
}

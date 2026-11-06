#pragma once
#include "Scene.h"

namespace Client
{
struct Map;
struct Tile;

struct GameScene : Scene, Broadcaster<GameOp>
{
	using Op = GameOp;

	GameScene();
	~GameScene();

	void NetOperationProcess() override;

	static constexpr auto kID = SceneID::Game;

	void Tick() override;
	void Render() override;
	bool HandleEvents(const sf::Event & ev) override;
	TilePoint GetMouseTile();

	sf::View _game_view;

	// A click whose press and release came between two frames (a browser's slow
	// frame, a test's click): taken as held for one Tick
	bool _click_left = false, _click_right = false;
};
}

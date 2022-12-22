#pragma once
#include "Scene.h"

namespace Client
{
class Map;
class Tile;

class GameScene : public Scene, public Broadcaster<GameOp>
{
public:
	using Op = GameOp;

	GameScene();
	~GameScene();

	void NetOperationProcess() override;

	static constexpr auto kID = SceneID::Game;

	void Tick() override;
	void Render(sf::RenderTarget& tgt, const sf::RenderStates& states) override;
	bool HandleEvents(const sf::Event & ev) override;
	TilePoint GetMouseTile();

	sf::View _game_view;

private:
	void NetOp_Action();

};
}
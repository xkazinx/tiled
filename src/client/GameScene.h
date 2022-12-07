#pragma once
#include "Scene.h"

namespace Client
{
class Map;
class Tile;

class GameScene : public Scene
{
public:
	GameScene();
	~GameScene();

	static constexpr auto kID = SceneID::Game;

	void Tick() override;
	void Render(sf::RenderTarget& tgt, const sf::RenderStates& states) const override;
	void HandleEvents(const sf::Event& ev) override;
};
}
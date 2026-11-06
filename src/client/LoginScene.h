#pragma once
#include "Scene.h"

namespace Client
{
struct LoginScene : Scene
{
	LoginScene();
	~LoginScene();

	static constexpr auto kID = SceneID::Login;

	void Tick() override;

	void Render() override;
	bool HandleEvents(const sf::Event & ev) override;
	void ProcessUI() override;

	// The text boxes' buffers (a name / password of up to 32 bytes)
	char _user[player::kUsernameSize + 1] = {};
	char _pass[player::kPasswordSize + 1] = {};

	// The last word from the server (connected, a refused login...)
	string _status = "Connecting...";
};
}

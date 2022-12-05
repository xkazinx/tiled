#pragma once
#include "Scene.h"

class LoginScene : public Scene
{
public:
	LoginScene();
	~LoginScene();
	
	static constexpr auto kID = SceneID::Login;

	void Tick() override;
	
	void Render(sf::RenderTarget& tgt, const sf::RenderStates& states) const override;
	void HandleEvents(const sf::Event& ev) override;
	void ProcessUI() const override;

	kazin::Auth _auth;
};
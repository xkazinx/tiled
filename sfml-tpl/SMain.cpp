#include "stdafx.h"
#include "SMain.h"
#include "SceneMgr.h"
#include "TextMgr.h"
#include "Button.h"
#include "Background.h"
#include "Camera.h"

SMain::SMain() : Scene(ID::Main)
{
	_button = AddComponent<Button>(
		{
			mk<Button>(
				[&](const auto & event) {
					g_scene->Set(scene::ID::Game);
				},
				resolution::GetCenter( { 100.f, 50.f } ),
				"Start Game"
			)
		}
	);

	_background = mk<Background>(512, Vector2f(1000.f, g_res_y));
}

SMain::~SMain()
{
}

void SMain::Reset()
{

}

void SMain::Update(const sf::Time & delta)
{
	g_camera->move({ 0.05f * delta.asMilliseconds(), 0.f });
}

void SMain::Render(TargetStatesRef && ts) const
{
	ts().draw(*_background.get(), ts.states);
}

void SMain::HandleEvent(const sf::Event & event)
{
	if (event.type == sf::Event::KeyReleased) {
		if (event.key.code == sf::Keyboard::Enter) {
			g_scene->Set(ID::Game);
		}
	}
}

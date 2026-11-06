#include "stdafx.h"
#include "Scenes.h"
#include "LoginScene.h"
#include "GameScene.h"

namespace Client
{
Scenes::Scenes()
{
	Set(ID::Login);
}

Scenes::~Scenes()
{
	if (_mgr._current)
		_mgr._current->Deinitialize();
}

void Scenes::Set(ID id)
{
	unique_ptr<Scene> next;

	using enum ID;
	switch (id)
	{
	case Login:	next = make_unique<LoginScene>();	break;
	case Main:											break;
	case Lobby:											break;
	case Game:	next = make_unique<GameScene>();	break;
	case Result:										break;
	default:
		break;
	}

	if (!next)
		return;

	// The old scene leaves (Deinitialize) before it goes
	_mgr.Set(*next);
	_ptr = std::move(next);
}

void Scenes::Render()
{
	_mgr.Render();
	_ptr->ProcessUI();
}

void Scenes::Tick()
{
	_mgr.Tick();
}

void Scenes::HandleEvent(const sf::Event * ev)
{
	_mgr.HandleEvent(ev);
}
}

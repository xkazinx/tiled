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
	
}

void Scenes::Set(ID id)
{
	using enum ID;
	switch(id)
	{
	case Login:	_ptr = make_unique<LoginScene>();	break;
	case Main:													break;
	case Lobby:													break;
	case Game:	_ptr = make_unique<GameScene>();		break;
	case Result:											break;
	default: 
		break;
	}
}

void Scenes::Render(sf::RenderTarget& tgt, const sf::RenderStates& states) const
{
	_ptr->Render(tgt, states);
	_ptr->ProcessUI();
}

void Scenes::Tick()
{
	_ptr->Tick();
}

bool Scenes::HandleEvent([[maybe_unused]] const sf::Event& ev)
{
	return _ptr->HandleEvents(ev);
}
}
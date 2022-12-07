#include "stdafx.h"
#include "SceneMgr.h"
#include "LoginScene.h"
#include "GameScene.h"

namespace Client
{
SceneMgr::SceneMgr()
{
	Set(ID::Login);
}

SceneMgr::~SceneMgr()
{
	
}

void SceneMgr::Set(ID id)
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

void SceneMgr::Render(sf::RenderTarget& tgt, const sf::RenderStates& states) const
{
	_ptr->Render(tgt, states);
	_ptr->ProcessUI();
}

void SceneMgr::Tick()
{
	
}

bool SceneMgr::HandleEvent([[maybe_unused]] const sf::Event& ev)
{
	return _ptr->HandleEvents(ev);
}
}
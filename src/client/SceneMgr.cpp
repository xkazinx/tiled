#include "stdafx.h"
#include "SceneMgr.h"
#include "LoginScene.h"
#include "GameScene.h"

SceneMgr::SceneMgr()
{
	Set(ID::Login);
}

SceneMgr::~SceneMgr()
{
	
}

void SceneMgr::Set(ID id)
{
	switch(id)
	{
	case ID::Login: _ptr = make_unique<LoginScene>(); break;
	case ID::Main: break;
	case ID::Lobby: break;
	case ID::Game: _ptr = make_unique<GameScene>(); break; break;
	case ID::Result: break;
	default: break;
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

bool SceneMgr::HandleEvent(const sf::Event& ev)
{
	return false;
}

#include "stdafx.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "Camera.h"

#include "SMain.h"
#include "SGame.h"

SceneMgr::SceneMgr()
{
	// #scene create
	At(ID::Main) = mk<SMain>();
	At(ID::Game) = mk<SGame>();

	Set(ID::Main);
}

SceneMgr::~SceneMgr()
{

}

void SceneMgr::Set(const ID & id) {
	auto last_id = _current ? _current->_id : ID::Max;

	_current = At(id).get();

	if (last_id == ID::Max || _current->_id != last_id) {
		g_camera->setCenter({ g_res_x / 2.f, g_res_y / 2.f });
		_current->Reset();
	}

	g_camera->BindScene(_current);
}

void SceneMgr::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(*_current, states);
}

void SceneMgr::Update(const sf::Time & delta) {
	_current->Update(delta);
}

void SceneMgr::HandleEvent(const sf::Event & event) {
	_current->HandleEvents(event);
}
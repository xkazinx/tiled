#include "stdafx.h"
#include "SGame.h"
#include "TextMgr.h"
#include "Background.h"
#include "Camera.h"
#include "Player.h"
#include "Random.h"
#include "Enemy.h"
#include "Projectile.h"

SGame::SGame() : Scene(ID::Game)
{

}

SGame::~SGame()
{
}

void SGame::Reset()
{
	_objects.clear();
	_objects.emplace_back(mk<Background>(1024, Vector2f(800.f, g_res_y)));
	_objects.emplace_back(mk<Player>(Vector2f{ 50.f, g_res_y / 2.f }));

	_proyectiles_hit = _proyectiles_missed = enemies_killed = enemies_missed = 0;
}

void SGame::Update(const sf::Time & delta)
{
	Vector2f velocity(0.05f, 0.f);
	g_camera->move(DeltaVelocity(velocity, delta));
	
	for (auto it = _objects.begin(); it != _objects.end();)
	{
		auto & obj = *it;
		
		obj->Update(delta);

		for (auto col_it = it;/*_objects.begin() + (it - _objects.begin());*/ col_it != _objects.end(); ++col_it)
		{
			if (*it == *col_it)
				continue;

			if ((*it)->CollidesWith(col_it->get())) {
				(*it)->HandleCollition(col_it->get());
				(*col_it)->HandleCollition(it->get());
			}
		}

		if (obj->HasExpired()) {
			it = _objects.erase(it);
			enemies_missed++;
		}
		else ++it;
	}
}

void SGame::Render(TargetStatesRef && ts) const
{
	for (auto & obj : _objects) {
		ts().draw(*obj.get(), ts.states);
	}

	g_app->_window.setView(g_app->_window.getDefaultView());
	g_text->DrawText("Enemies killed: 50/" + std::to_string(enemies_missed), {10.f, 10.f});
	g_text->DrawText("Projectiles hit: 10/100", {10.f, 40.f});
	/*sf::RectangleShape rectangle({ _player_bounds.width, _player_bounds.height });

	rectangle.setPosition(_player_bounds.left, _player_bounds.top);

	rectangle.setFillColor(sf::Color(255, 255, 255, 150));

	ts().draw(rectangle, ts.states);*/
}

void SGame::HandleEvent(const sf::Event & event)
{
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == Key::E && Key::isKeyPressed(Key::LControl)) {
			_objects.push_back(mk<Enemy>(Vector2f{ 
				g_camera->GetRight() + 100.f, 
				g_rand->Generate<float>(0.f, g_res_y) 
			}));
		}
		else if (event.key.code == Key::A) {
			_objects.push_back(mk<Projectile>(
				GetPlayer()->getPosition()
			));
		}
	}
}

Player * SGame::GetPlayer()
{
	return dynamic_cast<Player*>(_objects[1].get());
}

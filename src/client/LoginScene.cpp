#include "stdafx.h"
#include "LoginScene.h"

LoginScene::LoginScene() : 
	_circle(50.f, 50)
{
	_circle.setFillColor(sf::Color::Red);
}


LoginScene::~LoginScene()
{

}

void LoginScene::Tick()
{
	/*static bool HasLogged = false;
	if (!HasLogged)
	{
		HasLogged = true;

		string password;

		cout << "Username: ";
		cin >> _username;
		cout << "Password: ";
		cin >> password;
	}*/


	/*ImGui::Begin("Hello, world!");
	ImGui::Button("Look at this pretty button");
	ImGui::End();*/
}

void LoginScene::Render(sf::RenderTarget& tgt, const sf::RenderStates& states) const
{
	tgt.draw(_circle, states);
}

void LoginScene::HandleEvents(const sf::Event& ev)
{

}


#include "stdafx.h"
#include "LoginScene.h"

LoginScene::LoginScene() : 
	_circle(50.f, 50)
{
	_circle.setFillColor(sf::Color::Red);
	_username.reserve(64);
	_password.reserve(64);
}


LoginScene::~LoginScene()
{

}

void LoginScene::Tick()
{
	/*if (ImGui::BeginMenu("Login"))
	{
		//ShowExampleMenuFile();
		ImGui::EndMenu();
	}*/
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


	ImGui::Begin("Hello, world!");
	ImGui::Button("Look at this pretty button");
	ImGui::End();
}

void LoginScene::ProcessUI() const
{
	ImGui::Begin("Hello, world!");
	ImGui::InputText("Username", (char*)_username.data(), _username.capacity());
	ImGui::InputText("Password", (char*)_password.data(), _username.capacity(), ImGuiInputTextFlags_Password);

	if (ImGui::Button("Login"))
	{
		if (!g_sock->Connect("localhost", 321))
		{
			cout << "Couldn't connect" << endl;
		}
	}

	ImGui::End();
}

void LoginScene::Render(sf::RenderTarget& tgt, const sf::RenderStates& states) const
{
	tgt.draw(_circle, states);
}

void LoginScene::HandleEvents(const sf::Event& ev)
{

}


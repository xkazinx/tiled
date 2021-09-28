#include "stdafx.h"
#include "LoginScene.h"
#include "App.h"

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
}

void LoginScene::ProcessUI() const
{
	ImGui::Begin("Hello, world!");
	ImGui::InputText("Username", (char*)_username, sizeof(_username));
	ImGui::InputText("Password", (char*)_password, sizeof(_password), ImGuiInputTextFlags_Password);

	if (ImGui::Button("Login"))
	{
		Log::Out("Sent Login info: User: {} - Pass: {}", _username, _password);
		
		g_app->Send(MainOp::Login, 
			NetString(_username, player::kNameSize),
			NetString(_password, player::kPasswordSize)
		);
	}

	ImGui::End();
}

void LoginScene::Render(sf::RenderTarget& tgt, const sf::RenderStates& states) const
{
	tgt.draw(_circle, states);
}

void LoginScene::HandleEvents([[maybe_unused]] const sf::Event& ev)
{

}


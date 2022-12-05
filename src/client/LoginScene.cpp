#include "stdafx.h"
#include "LoginScene.h"
#include "App.h"

LoginScene::LoginScene()
{
	_auth._user.resize(player::kUsernameSize);
	_auth._pass.resize(player::kPasswordSize);
}


LoginScene::~LoginScene()
{

}

void LoginScene::Tick()
{
}

void LoginScene::ProcessUI() const
{
	ImGui::Begin("Authentication");
	ImGui::InputText("Username", (char*)_auth._user.data(), player::kUsernameSize + 1);
	ImGui::InputText("Password", (char*)_auth._pass.data(), player::kPasswordSize + 1, ImGuiInputTextFlags_Password);

	if (ImGui::Button("Login"))
	{
		PrintDebug("Sent Login Auth{}", Format(_auth));
		g_app->Send(MainOp::Login, _auth);

		// #test sending and reading with size 32 but written < 32 with char and string
		// #test sending and reading with size 32 and written 32 with char and string
	}
	
	if (ImGui::Button("Create Account"))
	{
		PrintDebug("Sent Create Account Auth{}", Format(_auth));
		g_app->Send(MainOp::CreateAccount, _auth);
	}

	ImGui::End();
}

void LoginScene::Render(sf::RenderTarget& tgt, const sf::RenderStates& states) const
{

}

void LoginScene::HandleEvents([[maybe_unused]] const sf::Event& ev)
{

}


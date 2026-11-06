#include "stdafx.h"
#include "LoginScene.h"
#include "App.h"

namespace Client
{
LoginScene::LoginScene()
{
	// Launch flags user= / pass= fill the boxes (tests, the web page's query string)
	const string user = lib::init::Value("user"), pass = lib::init::Value("pass");
	user.copy(_user, player::kUsernameSize);
	pass.copy(_pass, player::kPasswordSize);
}


LoginScene::~LoginScene()
{

}

void LoginScene::Tick()
{
}

void LoginScene::ProcessUI()
{
	ImGui::SetNextWindowPos({ 362.f, 284.f }, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize({ 300.f, 140.f }, ImGuiCond_FirstUseEver);
	ImGui::Begin("Authentication");
	ImGui::InputText("Username", _user, sizeof(_user));
	ImGui::InputText("Password", _pass, sizeof(_pass), ImGuiInputTextFlags_Password);

	const Auth auth(_user, _pass);
	if (ImGui::Button("Login"))
	{
		DebugLog("Sent Login Auth{}", Format(auth));
		g_app->Send(MainOp::Login, auth);
	}

	ImGui::SameLine();
	if (ImGui::Button("Create Account"))
	{
		DebugLog("Sent Create Account Auth{}", Format(auth));
		g_app->Send(MainOp::CreateAccount, auth);
	}

	ImGui::TextWrapped("%s", _status.c_str());

	ImGui::End();
}

void LoginScene::Render()
{

}

bool LoginScene::HandleEvents([[maybe_unused]] const sf::Event & ev)
{
	return false;
}
}

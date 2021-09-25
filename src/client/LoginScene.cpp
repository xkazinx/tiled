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
	ImGui::InputText("Username", (char*)_username, sizeof(_username));
	ImGui::InputText("Password", (char*)_password, sizeof(_password), ImGuiInputTextFlags_Password);

	if (ImGui::Button("Login"))
	{
		cout << "User: " << _username << " Pass: " << _password << endl;
		
		//g_sock->Send(CreateStatic(Op::Login, _username, _password));
		//auto out = OutPkt::CreateDynamic(NetOp::Login,sizeof(_username) + sizeof(_password));
		string str1 = "hola";
		string str2 = "bro";
		int algo = 3;
		g_app->Send(MainOp::Login, algo, packet::SerializableString(str1, 2), packet::SerializableString(str2));

		/*
		 *auto out = OutPkt::CreateDynamic(NetOp::Main, sizeof(string("hola")) + sizeof(string("bro")));
		out->Write(string("hola"));
		out->Write(string("bro"));
		g_sock->Send(out);
		*/
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


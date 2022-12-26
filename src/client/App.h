#pragma once

namespace Client
{
class App final : public kazin::app::Base, public Broadcaster<MainOp>
{
public:
	using Op = MainOp;

	App();
	~App();

	void NetOperationProcess() override;

	int Tick() override;

	// false if the processed event has closed the window
	bool HandleEvents();
	
	// #todo make dynamic resolution and scale, make it readable from config file, and changeable in game

private:
	void InitializeWindow();

	void OnConnect(ENetEvent & ev);
	void OnPacket(ENetEvent & ev);
	void OnDisconnect(ENetEvent & ev);

	void NetOp_Login();
	void NetOp_CreateAccount();
	void NetOp_EnterGame();
};
}
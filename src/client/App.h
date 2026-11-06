#pragma once

namespace Client
{
struct App final : Broadcaster<MainOp>
{
	using Op = MainOp;

	App();
	~App();

	void NetOperationProcess() override;

	int Tick();

	// One pass of the loop: events, network, update, render; false once the window closed
	bool Frame(sf::Clock & delta_clock);

	// false if the processed event has closed the window
	bool HandleEvents();

	// #todo make dynamic resolution and scale, make it readable from config file, and changeable in game

	sf::RenderWindow _window;
	u16 _res_x, _res_y;

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

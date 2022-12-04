#pragma once
#include "Broadcaster.h" // #why do I need to include this?

class App final : public Broadcaster<MainOp>
{
public:
	using Op = MainOp;

	App();
	~App();

	void NetOperationProcess() override;

	int Tick();

	// false if the processed event has closed the window
	bool HandleEvents();

	RenderWindow _window;
	
	// #todo make dynamic resolution and scale, make it readable from config file, and changeable in game
	sf::Vector2<i16> _res = { 1024, 768 };

private:
	void InitializeWindow();
	void AdjustViewport(const GLsizei& x, const GLsizei& y);

	void OnConnect(ENetEvent& ev);
	void OnPacket(ENetEvent& ev);
	void OnDisconnect(ENetEvent& ev);

	void NetOp_Login();
	void NetOp_CreateAccount();
};

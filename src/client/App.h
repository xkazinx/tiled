#pragma once
#include "Broadcaster.h" // #why do I need to include this?

class App final : public Broadcaster<MainOp>
{
public:
	App();
	~App();

	void NetOperationProcess() override;

	int Tick();

	// false if the processed event has closed the window
	bool HandleEvents();

	RenderWindow _window;

	void HandleSocket();

private:
	void InitializeWindow();
	void AdjustViewport(const GLsizei& x, const GLsizei& y);

	void OnConnect(ENetEvent& ev);
	void OnPacket(ENetEvent& ev);
	void OnDisconnect(ENetEvent& ev);
};

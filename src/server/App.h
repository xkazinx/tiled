#pragma once

class Player;

class App
{
public:
	App();
	~App();

	int Tick();
	void HandleSocket();
	void OnConnect(ENetEvent& ev);
	void OnPacket(ENetEvent& ev);
	void OnDisconnect(ENetEvent& ev);
	Player* OnAuth(const ENetEvent& ev) const;

private:
	void NetOp_Login();
};


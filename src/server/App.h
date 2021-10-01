#pragma once

class Player;

class App final : Broadcaster<MainOp>
{
public:
	using Op = MainOp;

	App();
	~App();

	int Tick();
	void OnConnect(ENetEvent & ev);
	void OnPacket(ENetEvent & ev);
	void OnDisconnect(ENetEvent & ev);
	[[nodiscard]] Player* OnAuth(const ENetEvent & ev) const;

	void NetOperationProcess() override;

private:
	void NetOp_Login();
	void NetOp_CreateAccount();
};


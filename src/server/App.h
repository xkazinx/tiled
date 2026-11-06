#pragma once
#include <net/ws_gateway.h>

namespace Server
{
struct Player;

struct App final : Broadcaster<MainOp>
{
	using Op = MainOp;

	App();
	~App();

	int Tick();
	void OnConnect(ENetEvent & ev);
	void OnPacket(ENetEvent & ev);
	void OnDisconnect(ENetEvent & ev);
	[[nodiscard]] Player * OnAuth(const ENetEvent & ev) const;

	void NetOperationProcess() override;

	// Browser clients (the web build) in through WebSockets, as ENet peers of g_sock
	// (kazilib net/ws_gateway.h): launch flags ws=<port> (default 7030, 0 = off),
	// ws_bind=<address> (default 127.0.0.1: a reverse proxy in front), web=<folder>
	// (the web build's files, served on the same port; default the exe's web\ folder
	// when there is one)
	lib::ws::Gateway _ws;

private:
	void NetOp_Login();
	void NetOp_CreateAccount();
	void NetOp_EnterGame();
};
}

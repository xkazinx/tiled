#pragma once

#include <string> // remove
using namespace std;

class Socket
{
public:
	enum Type : u8
	{
		Client,
		Server
	};

	Socket(Type type, u8 peers = 0);
	~Socket();

	static u8 Count;
	
	ENetHost* _host;
	Type _type;

	auto IsClient() const { return _type == Type::Client; }
	bool Service(ENetEvent& ev);

	// Client only
	ENetPeer* _peer;

	bool Connect(const string& ip, u16 port);
};
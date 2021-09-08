#pragma once

class Socket
{
public:
	enum Type : u8
	{
		Client,
		Server
	};

	Socket(Type type, szt peers = 0);
	~Socket();
	
	static u8 Count;
	
	ENetHost* _host;
	Type _type;

	auto IsClient() const { return _type == Type::Client; }

	// Must call enet_packet_destroy(ev.packet); after processing packet
	bool Service(ENetEvent& ev);

	// Client only
	ENetPeer* _peer;

	bool Connect(const string& ip, u16 port);
};
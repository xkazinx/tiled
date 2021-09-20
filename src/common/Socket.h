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
	
	void Send(OutPktRef out, enet_uint8 channel = 0u) const;
	void Send(ENetPeer* peer, OutPktRef out, const enet_uint8 channel) const;
};

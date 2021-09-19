#include "stdafx.h"
#include "Socket.h"

u8 Socket::Count = 0;

Socket::Socket(Type type, szt peers) : _type(type)
{
	if (Count++ == 0)
	{
		enet_initialize();
	}

	// For server only
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = 321;
	
	_host = enet_host_create(IsClient() ? nullptr : &address, /* create a client host */
		IsClient() ? 1 : peers /* only allow 1 outgoing connection */,
		1 /* allow up 2 channels to be used, 0 and 1 */,
		0 /* assume any amount of incoming bandwidth */,
		0 /* assume any amount of outgoing bandwidth */);
	
	if (!_host)
	{
		cout << "Error on Socket::Socket()";
	}
}

Socket::~Socket()
{
	if (_host)
	{
		enet_host_destroy(_host);
	}

	if (--Count == 0)
	{
		enet_deinitialize();
	}

}
bool Socket::Service(ENetEvent& ev)
{
	if (enet_host_service(_host, &ev, 0) != 0)
	{
		switch (ev.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
		{
			constexpr auto ip_size = 3 * 4 + 3;
			array<char, ip_size + 1> ip;
			enet_address_get_host_ip(&ev.peer->address, ip.data(), ip.size());

			cout << "A new client connected from " << ip.data() << 
				" (" << ev.peer->address.host << ")" <<
				":" << ev.peer->address.port << endl;
			
			ev.peer->data = nullptr;
			return true;
		}

		case ENET_EVENT_TYPE_RECEIVE:
			printf("A packet of length %u was received from %s on channel %u.\n",
				ev.packet->dataLength,
				(const char*)ev.peer->data,
				ev.channelID);
			return true;

		case ENET_EVENT_TYPE_DISCONNECT:
			printf("%s disconnected.\n", (const char*)ev.peer->data);
			ev.peer->data = nullptr;
			return true;
		}
	}

	return false;
}

bool Socket::Connect(const string& ip, u16 port)
{
	ENetAddress address;
	
	enet_address_set_host(&address, ip.c_str());
	address.port = port;

	return (_peer = enet_host_connect(_host, &address, 1, 0)) != nullptr;
}
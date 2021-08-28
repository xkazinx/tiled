#include "stdafx.h"
#include "Socket.h"

u8 Socket::Count = 0;

Socket::Socket(Type type, u8 peers) : _type(type)
{
	if (Count++ == 0)
	{
		enet_initialize();
	}

	// For server only
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = 1234;

	_host = enet_host_create(IsClient() ? nullptr : &address, /* create a client host */
		IsClient() ? 0 : peers /* only allow 1 outgoing connection */,
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
	if (enet_host_service(_host, &ev, 1) > 0)
	{
		switch (ev.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
			printf("A new client connected from %x:%u.\n",
				ev.peer->address.host,
				ev.peer->address.port);
			
			ev.peer->data = nullptr;
			return true;

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

	/* Connect to some.server.net:1234. */
	enet_address_set_host(&address, ip.c_str());
	address.port = port;

	/* Initiate the connection, allocating the two channels 0 and 1. */
	_peer = enet_host_connect(_host, &address, 1, 0);

	return _peer != nullptr;
}
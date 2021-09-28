#include "stdafx.h"
#include "socket.h"

namespace common
{
u8 Socket::Count = 0;

Socket::Socket(Type type, szt peers)
	: _type(type)
{
	if (Count++ == 0)
	{
		enet_initialize();
	}

	// For server only
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = 321;
	
	_host = enet_host_create(IsClient() ? nullptr : &address, 
		IsClient() ? 1 : peers, 1, 0, 0);
	
	if (!_host)
	{
		Log::Err("enet_host_create failure");
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
bool Socket::Service(ENetEvent & ev) const
{
	if (enet_host_service(_host, &ev, 0) != 0)
	{
		switch (ev.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
			Log::Out("Client {}:{} connected", 
				FetchIP(ev.peer), ev.peer->address.port);
			ev.peer->data = nullptr;
			return true;

		case ENET_EVENT_TYPE_RECEIVE:
			return true;

		case ENET_EVENT_TYPE_DISCONNECT:
			if (ev.peer->data)
			{
				Log::Out("Client {}:{} disconnected", 
					FetchIP(ev.peer), ev.peer->address.port);
				ev.peer->data = nullptr;
			}
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

void Socket::Send(OutPktRef out, enet_uint8 channel /*= 0u*/) const
{
	out->Send(_peer, channel);
}

void Socket::Send(ENetPeer* peer, OutPktRef out, const enet_uint8 channel) const
{
	out->Send(peer, channel);
}

string Socket::FetchIP(ENetPeer* peer)
{
	char data[15]{};
	if (enet_address_get_host_ip(&peer->address, data, sizeof(data)) < 0)
	{
		Log::Err("Error on enet_address_get_host_ip");
	}

	return data;
}
}
#include "stdafx.h"
#include <Socket.h>

namespace common
{
namespace tile
{
#ifdef CLIENT
	ColorArray kColors =
	{
		sf::Color::Green,
		sf::Color::Blue,
		sf::Color{ 50, 50, 50, 255 }
	};
#endif
}

namespace packet
{
	Base::Base(ENetPacket* enet_packet, const Size sz)
		: _overflow(false), _enet_packet(enet_packet), _size(sz)
	{
		_ptr = (char*)_enet_packet->data;
	}

	bool Base::ProcessOverflow(const bool extra_condition)
	{
		_overflow = IsOverflow() || extra_condition;
		return IsOverflow();
	}

	void Base::_Move(const Size pos, const bool is_out)
	{
		_ptr += pos;

		if (is_out)
		{
			GetLength() += pos;
		}
		else
		{
			_size += pos;
		}
	}

	bool Base::IsOverflow() const
	{
		const auto res = _overflow;
#ifdef DEBUG_PACKET
		if (res)
		{
			cout << "Overflow met!" << endl;
		}
#endif
		return res;
	}

	std::string Base::GetIP(ENetPeer* peer)
	{
		char ip[56] = {};
		ZeroMemory(ip, sizeof(ip));
		enet_address_get_host_ip(&peer->address, ip, sizeof(ip));
		return std::string(ip);
	}

	Out::Out(const Size sz)
		: Base(enet_packet_create(nullptr, sz, ENET_PACKET_FLAG_RELIABLE), sz)
	{
		GetLength() = 0;
	}

	Out::~Out()
	{
		if (_enet_packet->referenceCount == 0)
		{
			enet_packet_destroy(_enet_packet);
		}
	}

	void Out::Send(ENetPeer* peer, const enet_uint8 channel)
	{
		if (!peer || GetLength() == 0 || IsOverflow())
			return;

		enet_peer_send(peer, channel, _enet_packet);
	}

	void Out::Send(class ::Socket* sock, const enet_uint8 channel /*= 0*/)
	{
		if (!sock->_peer)
			return;

		Send(sock->_peer, channel);
	}

	void Out::Move(const Size sz)
	{
		Base::_Move(sz, true);
	}

	bool Out::HasOverflowed(const Size  sz)
	{
		return ProcessOverflow(GetSize() - GetLength() < sz);
	}

	In::In(ENetPacket* p)
		: Base(p, 0)
	{

	}

	In::~In() {
		enet_packet_destroy(_enet_packet);
	}

	bool In::HasOverflowed(const Size sz)
	{
		return ProcessOverflow(GetLength() < GetSize() + sz);
	}

	void In::Move(const Size sz)
	{
		Base::_Move(sz, false);
	}
}
}
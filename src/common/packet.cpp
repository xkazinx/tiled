#include "stdafx.h"
#include "packet.h"

namespace common::packet
{
	string SerializableString::kDummy;

	Base::Base(ENetPacket* packet, const Size sz)
		: _ptr(reinterpret_cast<char*>(packet->data)), _size(sz),
		_enet_packet(packet), _overflow(false)
	{

	}

	bool Base::ProcessOverflow(const bool extra_condition)
	{
		return (_overflow = extra_condition);
	}

	void Base::_Move(const Size pos, const bool is_out)
	{
		static_cast<Size&>(is_out ? GetLength() : _size) += pos;
		_ptr += pos;
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

	void Out::Send(ENetPeer* peer, const enet_uint8 channel) const
	{
		if (!peer || GetLength() == 0 || _overflow)
		{
			cout << "Can't send packet: Peer unavailable, empty packet, or overflowed packet" << endl;
			return;
		}

		enet_peer_send(peer, channel, _enet_packet);
	}

	void Out::Move(const Size sz)
	{
		_Move(sz, true);
	}

	bool Out::HasOverflowed(const Size  sz)
	{
		return ProcessOverflow(GetSize() - GetLength() < sz);
	}

	In::In(ENetPacket* p)
		: Base(p, 0)
	{

	}

	In::~In()
	{
		enet_packet_destroy(_enet_packet);
	}

	bool In::HasOverflowed(const Size sz)
	{
		return ProcessOverflow(GetLength() < GetSize() + sz);
	}

	void In::Move(const Size sz)
	{
		_Move(sz, false);
	}

	void SerializableString::Write(Out & out) const
	{
		if (IsString() || IsChar())
		{
			const Size sz = static_cast<Size>(IsString() ? _ref.size() : strlen(_data));
			out.Write(sz);

			if (out.HasOverflowed(sz))
				return;

			char* data = IsString() ? &_ref[0] : (char*)_data[0];
			std::copy_n(data, sz, out.GetPointer<char>());
			out.Move(sz);
		}
		else if (IsFixedString() || IsFixedChar())
		{
			out.Write(_sz);

			if (out.HasOverflowed(_sz))
				return;

			std::copy_n(IsFixedString() ? &_ref[0] : (char*)_data[0], _sz, out.GetPointer<char>());
			out.Move(_sz);
		}
	}

	void SerializableString::Read(In* in)
	{
		if (IsString() || IsChar())
		{
			Size sz;
			in->Read(sz);

			if (in->HasOverflowed(sz))
				return;
			
			if (IsString())
			{
				_ref.resize(sz);
			}
			// #error checks

			std::copy_n(in->GetPointer<char>(), sz, IsString() ? &_ref[0] : (char*)_data[0]);
			in->Move(sz);
		}
		else if (IsFixedString() || IsFixedChar())
		{
			// #packet auto resize string 
			if (in->HasOverflowed(_sz))
				return;

			std::copy_n(in->GetPointer<char>(), _sz, IsString() ? &_ref[0] : (char*)_data[0]);
		}
	}

	SerializableString::~SerializableString() = default;
}
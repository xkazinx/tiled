#include "stdafx.h"
#include "packet.h"

namespace common::packet
{
	string SerializableString::kDummy;

	Base::Base(ENetPacket* packet, const Size sz)
		: _ptr((char*)packet->data), _size(sz),
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
		Size sz;
		if (IsString() || IsChar())
		{
			sz = static_cast<Size>(IsString() ? _ref.size() : strlen(_data));
		}
		else //if (IsFixedString() || IsFixedChar())
		{
			sz = _sz;
		}

		if (out.HasOverflowed(sz))
			return;

		//std::copy_n(IsString() || IsFixedString() ? _ref.data() : (char*)_data,
			//sz, out.GetPointer<char>());

		memcpy(out.GetPointer<char>(),
			IsString() || IsFixedString() ? _ref.data() : (char*)_data, sz);

		out.Move(sz);
	}

	void SerializableString::Read(In* in)
	{
		Size sz;
		if (IsString() || IsChar())
		{
			in->Read(sz);
		}
		else //if (IsFixedString() || IsFixedChar())
		{
			sz = _sz;
		}

		if (in->HasOverflowed(sz))
			return;

		if (IsString() || IsFixedString())
		{
			_ref.clear();

			if (_ref.capacity() < sz)
			{
				cout << "Resized packet string from " << _ref.capacity();
				_ref.reserve(sz);
				cout << " to " << _ref.capacity() << endl;
			}

			_ref.assign(in->GetPointer<char>(), sz);
		}
		else
		{
			memcpy((char*)_data, in->GetPointer<char>(), sz);
		}

		in->Move(sz);
	}

	packet::Size SerializableString::GetPacketSize() const
	{
		packet::Size sz = 0;
		if (IsFixedString() || IsFixedChar())
		{
			sz = _sz;
		}
		else if (IsString())
		{
			sz = sizeof(Size) + _ref.size();
		}
		else if (IsChar())
		{
			// #net check if has to add +1 for null termination or something
			sz = sizeof(Size) + strlen(_data);
		}
		
		cout << "SerializableString::GetPacketSize(): " << sz << endl;
		return sz;
	}

	SerializableString::~SerializableString() = default;
}
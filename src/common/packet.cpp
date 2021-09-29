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
			Log::Debug("enet_packet_destroy");
			enet_packet_destroy(_enet_packet);
		}
	}

	void Out::Send(ENetPeer* peer, const enet_uint8 channel) const
	{
		if (!peer || GetLength() == 0 || _overflow)
		{
			// #format add Format #todo option to do like... Format(const bool&, u8 format_type)... where you can do like True/False, or On/Off, etc.
			Log::Error("Can't send packet: Peer unavailable? ({}), EmptyPacket? (Length: {}), Overflow ({})", 
				peer != nullptr, GetLength(), _overflow);

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
		Size sz = 1;
		if (IsString() || IsChar())
		{
			sz += static_cast<Size>(IsString() ? _ref.size() : strlen(_data));
			out.Write(sz);
		}
		else //if (IsFixedString() || IsFixedChar())
		{
			if (IsFixedString())
			{
				sz += (Size)_ref.size();
			}
			else //if (IsFixedChar())
			{
				sz += (Size)strlen(_data);
			}
		}
		
		if (out.HasOverflowed(sz))
			return;

		cout << "Wrote NetString of size " << sz << endl;
		// #todo not optimum
		memcpy(out.GetPointer<char>(), 
			IsString() || IsFixedString() ? _ref.c_str() : string(_data).c_str(), sz);
		
		//std::copy_n(IsString() || IsFixedString() ? _ref.data() : (char*)_data,
			//sz, out.GetPointer<char>());

		out.Move(IsFixedChar() || IsFixedString() ? _sz + 1 : sz);
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
			sz = _sz + 1;
		}

		if (in->HasOverflowed(sz))
			return;

		if (IsString() || IsFixedString())
		{
			if (_ref.capacity() < sz)
			{
				_ref.reserve(sz);
			}
			
			_ref.clear();

			// #netstring #packet #todo If you send "hola\0chau" you only receive "hola" when reading with string,
			// could add an argument to determine behavior
			_ref.assign(in->GetPointer<char>(), strlen(in->GetPointer<char>()));
		}
		else
		{
			memcpy((char*)_data, in->GetPointer<char>(), sz);
		}

		in->Move(sz);
	}

	packet::Size SerializableString::GetPacketSize() const
	{
		packet::Size sz = 1;
		if (IsFixedString() || IsFixedChar())
		{
			sz += _sz;
		}
		else if (IsString())
		{
			sz += sizeof(Size) + _ref.size();
		}
		else if (IsChar())
		{
			// #net check if has to add +1 for null termination or something
			sz += sizeof(Size) + strlen(_data);
		}

#ifdef DEBUG_PACKET
		Print("SerializableString::GetPacketSize(): {}", sz);
		return sz;
#endif
	}

	SerializableString::~SerializableString() = default;
}
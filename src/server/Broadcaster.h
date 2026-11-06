#pragma once

namespace Server
{
struct Player;

// A family of packets under one NetOp (kazilib net/broadcaster.h): <NetOp><Op><fields>.
// NetOperation reads the Op and keeps who sent it while NetOperationProcess runs.
template <class Op>
struct Broadcaster : lib::BroadcasterBase<NetOp, Op, lib::PktSizing::Exact>
{
	using Base = lib::BroadcasterBase<NetOp, Op, lib::PktSizing::Exact>;
	using Peer = ENetPeer;

	explicit Broadcaster(const NetOp net_op) : Base(net_op) {}
	~Broadcaster() override = default;

	template <typename ... Args>
	void Send(const Player * player, Op op, const Args & ... args)
	{
		auto out = this->Pkt(op, args...);
		Global::Send(player, out);
	}

	// <NetOp><op><args...>, exactly their size
	template <typename ... Args>
	[[nodiscard]] OPktPtr StaticPacket(Op op, const Args & ... args) const
	{
		return this->Pkt(op, args...);
	}

	// <NetOp><op><args...> with room for `extra` bytes the caller writes after them
	template <typename ... Args>
	[[nodiscard]] OPktPtr DynamicPacket(Op op, const PktSz extra, const Args & ... args) const
	{
		return this->FixedPkt(extra, op, args...);
	}

	void NetOperation(IPkt * const in, Player * const player, ENetPeer * const peer)
	{
		if (!this->FetchOperation(in))
			return;

		_in = in;
		_player = player;
		_peer = peer;
		NetOperationProcess();
		_in = nullptr;
	}

	virtual void NetOperationProcess() = 0;

	// The packet being read, who sent it (NetOperationProcess)
	IPkt * _in = nullptr;
	Player * _player = nullptr;
	ENetPeer * _peer = nullptr;
};
}

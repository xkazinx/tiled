#pragma once

namespace Client
{
// A family of packets under one NetOp (kazilib net/broadcaster.h): <NetOp><Op><fields>.
// NetOperation reads the Op and keeps the packet while NetOperationProcess runs.
template <class Op>
struct Broadcaster : lib::BroadcasterBase<NetOp, Op, lib::PktSizing::Exact>
{
	using Base = lib::BroadcasterBase<NetOp, Op, lib::PktSizing::Exact>;
	using Peer = ENetPeer;

	explicit Broadcaster(const NetOp net_op) : Base(net_op) {}
	~Broadcaster() override {}

	template <typename ... Args>
	void Send(Op op, const Args & ... args) const
	{
		auto out = this->Pkt(op, args...);
		Global::Send(out);
	}

	void NetOperation(IPkt * const in)
	{
		if (!this->FetchOperation(in))
			return;

		_in = in;
		NetOperationProcess();
		_in = nullptr;
	}

	virtual void NetOperationProcess() = 0;

	// The packet being read (NetOperationProcess)
	IPkt * _in = nullptr;
};
}

#pragma once

namespace Client
{
template<class Op = kazin::broadcaster::DummyOp>
class Broadcaster : public kazin::broadcaster::Base<Op>
{
	kazin::broadcaster::InOperationBase _opt;

public:
	using Peer = ENetPeer;

	explicit Broadcaster(const NetOp net_op) : kazin::broadcaster::Base<Op>(net_op){}
	~Broadcaster() override {}

	template <typename ... Args>
	void Send(Op op, const Args&...args) const
	{
		auto out = kazin::packet::Out::CreateStatic(this->_net_op, op, std::forward<const Args>(args)...);
		Global::Send(out);
	}

	void NetOperation(kazin::packet::In & pkt)
	{
		pkt.Read(this->_op);
		_opt = kazin::broadcaster::InOperationBase(pkt);
		NetOperationProcess();
	}

	virtual void NetOperationProcess() = 0;

	auto GetPacket() const { return _opt.GetPacket(); }
};
}
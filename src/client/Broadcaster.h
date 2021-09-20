#pragma once

template<class Op = broadcaster::DummyOp>
class Broadcaster : public broadcaster::Base<Op>
{
public:
	using Peer = ENetPeer;

	explicit Broadcaster(const NetOp net_op) : broadcaster::Base<Op>(net_op){}
	~Broadcaster() override {}

	template <typename ... Args>
	void Send(Op op, const Args&...args) const
	{
		auto out = packet::Out::CreateStatic(this->_net_op, op, std::forward<const Args>(args)...);
		Global::Send(out);
	}

	void NetOperation(packet::In & pkt)
	{
		pkt.Read(this->_op);
		_opt = broadcaster::InOperationBase(pkt);
		NetOperationProcess();
	}

	virtual void NetOperationProcess() = 0;

	auto GetPacket() const { return _opt.GetPacket(); }

private:
	broadcaster::InOperationBase _opt;
};
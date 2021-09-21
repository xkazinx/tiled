#pragma once

namespace common::broadcaster
{
	class InOperationBase
	{
	public:
		InOperationBase() = default;
		explicit InOperationBase(InPkt & pkt) : _pkt(&pkt) {}
		auto GetPacket() const { return _pkt; }

	private:
		InPkt * _pkt = nullptr;
	};

	enum class DummyOp { None };

	template<class Op>
	class Base
	{
	public:
		using Peer = ENetPeer;

		explicit Base(const NetOp net_op) : _net_op(net_op) {}
		virtual ~Base() = default;

		auto GetOperation() const { return _op; }

		template<typename ... Args>
		packet::OutPacketPtr&& StaticPacket(Op op, Args &&... args) const
		{
			return std::move(OutPkt::CreateStatic(_net_op, op, args...)); // #why not forward here?
		}

		template<typename ... Args>
		packet::OutPacketPtr&& DynamicPacket(Op op, Args &&... args) const
		{
			return std::move(OutPkt::CreateDynamic(_net_op, op, std::forward<Args>(args)...));
		}

	protected:
		Op _op;
		NetOp _net_op;
	};
}


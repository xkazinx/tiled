#pragma once

class Player;

class InOperation : public broadcaster::InOperationBase
{
	Player* _player;
	ENetPeer* _peer;

public:
	auto GetPlayer() const { return _player; }
	auto GetPeer() const { return _peer; }

	InOperation() : _player(nullptr), _peer(nullptr){}
	InOperation(packet::In & pkt, Player * player, ENetPeer * peer)
		: InOperationBase(pkt), _player(player), _peer(peer){}
	InOperation(const InOperation& opt) : _player(opt.GetPlayer()), _peer(opt.GetPeer()){}
};

template<class Op>
class Broadcaster : public broadcaster::Base<Op>
{
	InOperation _opt;

public:
	using Peer = ENetPeer;

	explicit Broadcaster(const NetOp net_op) : broadcaster::Base<Op>(net_op){}
	~Broadcaster() override = default;

	template <typename ... Args>
	void Send(const Player* player, Op op, Args && ... args)
	{
		// #todo #packet make it so you can send the argument directly
		auto out = broadcaster::Base<Op>::StaticPacket(op, forward<Args>(args)...);
		Global::Send(player, out);
	}

	/*template <typename ... Args>
	void Send(const class Room* room, Op op, Args&& ... args)
	{
		Global::Send(room, StaticPacket(this->_net_op, op, std::forward<Args>(args)...));
	}*/

	void NetOperation(const InOperation& opt)
	{
		opt.GetPacket()->Read(this->_op);
		_opt = opt;
		NetOperationProcess();
	}

	virtual void NetOperationProcess() = 0;

	auto GetPlayer() const { return _opt.GetPlayer();	}
	auto GetPacket() const { return _opt.GetPacket();	}
	auto GetPeer()	  const { return _opt.GetPeer();		}
};
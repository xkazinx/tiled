#pragma once

namespace Server
{
class Tile;
class Unit;
class Units;

class Map : public map::Base<Tile>, public Broadcaster<GameOp>
{
public:
	using Op = GameOp;

	static u64 sUUID;

	// Initialize tpl
	Map(MapID id);
	Map(Map * map);

	u64 _uuid = 0;

	void Tick();

	bool SetUnit(Unit * unit);
	bool UnsetUnit(Unit * unit);

	vector<Player*> _players;
	void Write_EnterGame(OutPktRef pkt);

	// #todo can this method be private?
	void NetOperationProcess() override;

	void Send(packet::OutPktPtr & pkt);
	void SendNearby(packet::OutPktPtr & pkt, Unit * unit);
};
}
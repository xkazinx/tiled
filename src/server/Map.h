#pragma once

namespace Server
{
struct Tile;
struct Unit;
struct Units;

struct Map : map::Base<Tile>, Broadcaster<GameOp>
{
	using Op = GameOp;

	static u64 sUUID;

	// Initialize tpl
	Map(MapID id);
	Map(Map * map);

	u64 _uuid = 0;

	void Tick();

	bool SetUnit(Unit * unit);
	bool UnsetUnit(Unit * unit);

	vector<Player *> _players;
	void Write_EnterGame(OPktPtr & pkt);

	// #todo can this method be private?
	void NetOperationProcess() override;

	void Send(OPktPtr & pkt);
	void SendNearby(OPktPtr & pkt, Unit * unit);
};
}

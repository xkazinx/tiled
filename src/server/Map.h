#pragma once

namespace Server
{
class Tile;
class Unit;
class Players;

class Map : public map::Base<Tile>
{
public:
	static u64 sUUID;

	// Initialize tpl
	Map(MapID id);
	Map(Map * map);

	u64 _uuid = 0;

	bool SetUnit(Unit * unit);
	bool UnsetUnit(Unit * unit);

	vector<Player*> _players;
	void Write_EnterGame(OutPktRef pkt);
};
}
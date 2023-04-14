#include "stdafx.h"
#include "Map.h"
#include "Tile.h"
#include "Player.h"

namespace Server
{
	// #todo make uuid sync with DB for unique games
	u64 Map::sUUID = 0;

	Map::Map(MapID id) : Broadcaster<GameOp>(NetOp::Game)
	{
		_id = id;

		if(id == MapID::Battlefield)
		{
			_size = { 100, 100 };
		}

		// #todo make files to sync these things, send on the meanwhile
		for(TilePointValue y = 0; y < _size.y; ++y)
		{
			for(TilePointValue x = 0; x < _size.x; ++x)
			{
				_tiles.push_back(make_unique<Tile>(
					g_rand->Probability(0.75) 
					? TileType::Movable 
					: TileType::Blocked)
				);
			}
		}
	}

	Map::Map(Map * map)
		: _uuid(++sUUID), Broadcaster<GameOp>(NetOp::Game)
	{
		_id = map->_id;

		_size = map->_size;

		for(auto & tile : map->_tiles)
		{
			_tiles.push_back(make_unique<Tile>(tile->_type));
		}
	}

	void Map::Tick()
	{

	}

	bool Map::SetUnit(Unit* unit)
	{
		auto tile = GetTile(unit->_pt);
		if(!tile)
		{
			PrintError("Map::SetUnit::!tile at position {}", Format(unit->_pt));
			return false;
		}

		if(tile->_unit)
		{
			PrintError("Map::SetUnit::tile->_unit already set ({}) for unit ({})", 
				Format(tile->_unit->_pt), tile->_unit->_name
			);

			return false;
		}

		tile->_unit = unit;
		return true;
	}

	bool Map::UnsetUnit(Unit * unit)
	{
		auto tile = GetTile(unit->_pt);
		if (!tile)
		{
			PrintError("Map::UnsetUnit::!tile at position {}", Format(unit->_pt));
			return false;
		}

		if (!tile->_unit)
		{
			PrintError("Map::UnsetUnit::!tile->_unit not present to unset ({} {})",
				unit->_name, Format(unit->_pt)
			);

			return false;
		}

		if(tile->_unit != unit)
		{
			PrintError("Map::UnsetUnit: Unit is present ({} {}) but doesn't match owner ({} {})",
				tile->_unit->_name, Format(tile->_unit->_pt), 
				unit->_name, Format(unit->_pt)
			);

			return false;
		}

		tile->_unit = nullptr;
		return true;
	}

	void Map::Write_EnterGame([[maybe_unused]] OutPktRef pkt)
	{
		PrintDebug("Map::Write_EnterGame: Sending {} tiles", Format(_size));

		pkt->Write(_size);

		TilePoint pt;
		for(pt.y = 0; pt.y < _size.y; ++pt.y)
		{
			for(pt.x = 0; pt.x < _size.x; ++pt.x)
			{
				pkt->Write(GetTile(pt)->_type);
			}
		}
	}

	void Map::NetOperationProcess()
	{
		PrintDebug("NetOperationProcess GameOp{}", Format(GetOperation()));

		switch (GetOperation())
		{

		default:
			PrintError("Unknown GamenOp{}", Format(GetOperation()));
			break;
		}
	}

	void Map::Send(packet::OutPktPtr& out)
	{
		for(auto player : _players)
		{
			Global::Send(player, out);
		}
	}

	void Map::SendNearby(packet::OutPktPtr& out, Unit * unit)
	{
		// #todo add boundary checks to only notify to players that are nearby
		for (auto player : _players)
		{
			if(player == unit || !player->IsOnSight(unit))
				continue;

			Global::Send(player, out);
		}
	}
}

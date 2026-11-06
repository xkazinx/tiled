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

		if (id == MapID::Battlefield)
		{
			_size = { 100, 100 };
		}

		// #todo make files to sync these things, send on the meanwhile
		for (TilePointValue y = 0; y < _size.y; ++y)
		{
			for (TilePointValue x = 0; x < _size.x; ++x)
			{
				_tiles.push_back(make_unique<Tile>(
					Probability(0.75)
					? TileType::Movable
					: TileType::Blocked)
				);
			}
		}
	}

	Map::Map(Map * map)
		: Broadcaster<GameOp>(NetOp::Game), _uuid(++sUUID)
	{
		_id = map->_id;

		_size = map->_size;

		for (auto & tile : map->_tiles)
		{
			_tiles.push_back(make_unique<Tile>(tile->_type));
		}
	}

	void Map::Tick()
	{

	}

	bool Map::SetUnit(Unit * unit)
	{
		if (!IsValidPoint(unit->_pt))
		{
			Err("Map::SetUnit::!tile at position {}", Format(unit->_pt));
			return false;
		}

		auto tile = GetTile(unit->_pt);
		if (tile->_unit)
		{
			Err("Map::SetUnit::tile->_unit already set ({}) for unit ({})",
				Format(tile->_unit->_pt), tile->_unit->_name
			);

			return false;
		}

		tile->_unit = unit;
		return true;
	}

	bool Map::UnsetUnit(Unit * unit)
	{
		if (!IsValidPoint(unit->_pt))
		{
			Err("Map::UnsetUnit::!tile at position {}", Format(unit->_pt));
			return false;
		}

		auto tile = GetTile(unit->_pt);
		if (!tile->_unit)
		{
			Err("Map::UnsetUnit::!tile->_unit not present to unset ({} {})",
				unit->_name, Format(unit->_pt)
			);

			return false;
		}

		if (tile->_unit != unit)
		{
			Err("Map::UnsetUnit: Unit is present ({} {}) but doesn't match owner ({} {})",
				tile->_unit->_name, Format(tile->_unit->_pt),
				unit->_name, Format(unit->_pt)
			);

			return false;
		}

		tile->_unit = nullptr;
		return true;
	}

	void Map::Write_EnterGame(OPktPtr & pkt)
	{
		DebugLog("Map::Write_EnterGame: Sending {} tiles", Format(_size));

		pkt->Write(_size);

		TilePoint pt;
		for (pt.y = 0; pt.y < _size.y; ++pt.y)
		{
			for (pt.x = 0; pt.x < _size.x; ++pt.x)
			{
				pkt->Write(GetTile(pt)->_type);
			}
		}
	}

	void Map::NetOperationProcess()
	{
		DebugLog("NetOperationProcess GameOp{}", Format(_op));

		switch (_op)
		{

		default:
			Err("Unknown GameOp{}", Format(_op));
			break;
		}
	}

	void Map::Send(OPktPtr & out)
	{
		for (auto player : _players)
		{
			Global::Send(player, out);
		}
	}

	void Map::SendNearby(OPktPtr & out, Unit * unit)
	{
		// #todo add boundary checks to only notify to players that are nearby
		for (auto player : _players)
		{
			if (player == unit || !player->IsOnSight(unit))
				continue;

			Global::Send(player, out);
		}
	}
}

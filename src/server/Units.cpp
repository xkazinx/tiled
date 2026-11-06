#include "stdafx.h"
#include "Units.h"
#include "Player.h"
#include "Map.h"
#include "Tile.h"

namespace Server
{
Units::Units() : Broadcaster<UnitOp>(NetOp::Unit)
{

}

Units::~Units()
{

}

void Units::NetOperationProcess()
{
	DebugLog("NetOperationProcess UnitOp{}", Format(_op));

	switch (_op)
	{
		OP(Action);

	default:
		Err("Unknown UnitOp{}", Format(_op));
		break;
	}
}

void Units::NetOp_Action()
{
	unit::Action action;
	Direction dir;
	TilePoint pt;

	_in->Read(action, pt, dir);

	auto p = _player;
	auto map = p->_map;

	if (_in->IsOverflow() || !dir.IsValid() || (action != unit::Action::Stop && action != unit::Action::Move) || !map->IsValidPoint(pt))
	{
		Err("NetOp_Action: Invalid incoming packet values");
		return;
	}

	if (p->_is_dead)
		return;

	// #todo add move speedhack check
	//player->_last_action_tm = time_;

	if (action == unit::Action::Move)
		map->UnsetUnit(p);

	p->_action = action;
	p->_dir = dir;

	if (action == unit::Action::Move)
	{
		p->_dir.Move(p->_pt);

		if (pt != p->_pt)
		{
			Err("NetOp_Action: received point is different from current player point");
			return;
		}

		if (action == unit::Action::Move)
			map->SetUnit(p);
	}

	DebugLog("Units::NetOp_Action: Action: {}, Dir: {}, Pt: {}",
		enum_name(action), enum_name(dir._id), Format(pt));

	// #todo CHECK HB move motion to see if must take something into account

	auto pkt = StaticPacket(UnitOp::Action, (unit::Base &)*_player);
	map->SendNearby(pkt, p);
}
}

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
	PrintDebug("NetOperationProcess UnitOp{}", Format(GetOperation()));

	switch (GetOperation())
	{
		OP(Action);

	default:
		PrintError("Unknown UnitOp{}", Format(GetOperation()));
		break;
	}
}

void Units::NetOp_Action()
{
	unit::Action action;
	Direction dir;
	TilePoint pt;

	GetPacket()->Read(action, pt, dir);

	auto p = GetPlayer();
	auto map = p->_map;

	if(!dir.IsValid() || !enum_contains(action) || !map->IsValidPoint(pt))
	{
		PrintError("NetOp_Action: Invalid incoming packet values");
		return;
	}
	
	if(p->_is_dead)
		return;

	// #todo add move speedhack check

	//player->_last_action_tm = time_;

	map->UnsetUnit(p);
	
	p->_action = action;
	p->_dir = dir;
	p->_dir.Move(p->_pt);

	if (pt != p->_pt)
	{
		PrintError("NetOp_Actiod: received point is different from current player point");
		return;
	}

	map->SetUnit(p);

	PrintDebug("Units::NetOp_Action: Action: {}, Dir: {}, Pt: {}",
		enum_name(action), enum_name(dir._id), Format(pt));

	// #todo CHECK HB move motion to see if must take something into account

	auto pkt = StaticPacket(UnitOp::Action, (unit::Base&)*GetPlayer());
	map->SendNearby(pkt, p);
}
}
#include "stdafx.h"
#include "Units.h"
#include "Player.h"
#include "Map.h"

namespace Client
{
Units::Units() : Broadcaster<UnitOp>(NetOp::Unit)
{

}
void Units::NetOperationProcess()
{
	DebugLog("NetOperationProcess UnitOp{}", Format(_op));

	switch (_op)
	{
		OP(Action); // #todo move to PlayerMgr(Server) / Player(Client)?
		OP(Spawn);
		OP(Remove);

	default:
		Err("Unknown UnitOp{}", Format(_op));
		break;
	}
}

void Units::Tick()
{
	// #todo move this
	for (auto unit : g_map->_units)
	{
		if (!unit->_is_moving &&
			(!unit->IsSelf() || (unit->IsSelf() && unit->_pt != g_player->_next_pt)))
		{
			if (unit->_stop_dir != Direction::None)
			{
				unit->_dir = unit->_stop_dir;
				unit->_stop_dir = Direction::None;
				unit->_action = unit::Action::Stop;

				g_map->SetUnit(unit);

				if (unit->IsSelf())
				{
					g_units->Send(UnitOp::Action,
						g_player->_action, g_player->_pt, g_player->_dir
					);
				}
			}
			else if (unit->IsSelf())
			{
				auto dir = g_player->GetNextDir(
					unit->_pt, g_player->_next_pt
				);

				if (dir == Direction::None)
					return;

				unit->_dir = dir;
				unit->_action = unit::Action::Move;

				g_map->SetUnit(unit);

				g_units->Send(UnitOp::Action,
					g_player->_action, g_player->_pt, g_player->_dir
				);
			}
		}
	}
}


void Units::NetOp_Spawn()
{
	unit::Base base;
	_in->Read(base);
	DebugLog("NetOp_Spawn {} {} {}", base._name, base._idx, Format(base._pt));
	g_map->SetUnit(&base);
}

void Units::NetOp_Action()
{
	unit::Base base;
	_in->Read(base);

	DebugLog("NetOp_Action {} {} {}", base._name, base._idx, Format(base._pt));

	g_map->SetUnit(&base);
}

void Units::NetOp_Remove()
{
	g_map->RemoveUnit(_in->ReadAs<unit::Idx>());
}
}

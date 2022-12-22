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
	PrintDebug("NetOperationProcess UnitOp{}", Format(GetOperation()));

	switch (GetOperation())
	{
		OP(Action); // #todo move to PlayerMgr(Server) / Player(Client)?

	default:
		PrintError("Unknown UnitOp{}", Format(GetOperation()));
		break;
	}
}
void Units::NetOp_Action()
{

}

void Units::Tick()
{
	if (auto* unit = g_player.get())
	{
		// #todo move to Player tick
		if (!unit->_is_moving && unit->_pt != unit->_next_pt)
		{
			PrintDebug("Map::Tick(): Point: {} | NextPoint: {}",
				Format(unit->_pt), Format(unit->_next_pt)
			);

			if (g_player->_stop_dir != Direction::None)
			{
				g_player->_dir = g_player->_stop_dir;
				g_player->_stop_dir = Direction::None;

				g_player->_action = unit::Action::Stop;

				g_map->SetUnit(g_player.get());

				g_units->Send(UnitOp::Action, 
					g_player->_action, g_player->_pt, g_player->_dir);
			}
			else
			{
				auto dir = g_player->GetNextDir(
					unit->_pt, unit->_next_pt
				);

				if (dir == Direction::None)
					return;

				unit->_dir = dir;
				unit->_action = unit::Action::Move;

				g_map->SetUnit(g_player.get());

				g_units->Send(UnitOp::Action, g_player->_action, g_player->_pt, g_player->_dir);
			}
		}
	}
}
}

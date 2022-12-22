#include "stdafx.h"
#include "Units.h"
#include "Player.h"

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

	PrintDebug("Units::NetOp_Action: Action: {}, Dir: {}, Pt: {}",
		enum_name(action), enum_name(dir._id), Format(pt));
}
}
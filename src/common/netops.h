#pragma once
// Every packet: <NetOp><its family's Op><fields> (kazilib net/broadcaster.h), u8
// each; values only appended.

namespace common
{
	enum class NetOp : u8
	{
		Main,
		Game,
		Unit,
		Disconnect,
	};

	enum class MainOp : u8
	{
		Login,			// c->s Auth; s->c bool accepted
		CreateAccount,	// c->s Auth; s->c bool created
		EnterGame,		// s->c idx, pt, map size + tiles, u16 count + units on sight
	};

	enum class GameOp : u8
	{
		Action,
	};

	enum class UnitOp : u8
	{
		Action,		// c->s action, pt, dir; s->c unit::Base (to the players near)
		Spawn,		// s->c unit::Base (a player entered, to the players near)
		Remove,		// s->c idx (a player left)
	};
}

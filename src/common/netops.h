#pragma once
#include <cassert>

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
		Login,
		CreateAccount,
		EnterGame,
	};

	enum class GameOp : u8
	{
		Action,
	};

	enum class UnitOp : u8
	{
		Action,
		Spawn
	};
}
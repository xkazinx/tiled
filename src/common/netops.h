#pragma once
#include <cassert>

namespace common
{
	enum class NetOp : u8
	{
		Main,
		Disconnect,
	};

	enum class MainOp : u8
	{
		Login,

	};
}
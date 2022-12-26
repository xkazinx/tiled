#include "stdafx.h"
#include "common.h"

#ifdef CLIENT
#include "../client/Player.h"
#endif

namespace common
{
namespace tile
{
	
}

namespace map
{

}

namespace unit
{
#ifdef CLIENT
	bool Base::IsSelf() const
	{
		return _idx == Client::Global::g_player->_idx;
	}
#endif
}

}

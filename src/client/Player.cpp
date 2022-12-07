#include "stdafx.h"
#include "Player.h"
#include "Map.h"

namespace Client
{
Direction Player::GetNextDir(TilePoint src, TilePoint dst)
{
	Direction dir, tmp_dir;
	if (src == dst) 
		return Direction::None;
	
	dir.SetNext(src, dst);

	auto get = [&](const auto & i)
	{
		Direction tmp_dir = (Direction::ID)i;

		if (tmp_dir._id >= Direction::Max)
			tmp_dir = Direction::Min;

		auto res = src + tmp_dir.GetDelta().As<TilePointValue>();

		if (g_map->IsMovable(res))
		{
			/*if (g_map->IsTeleport(res))
			{
				if (g_map->IsLocalMovable(res))
					return tmp_dir;
			}
			else */
			return tmp_dir;
		}

		return Direction{ Direction::None };
	};

	auto turn = [&](TilePoint next_pt)->int
	{
		Direction dir;
		auto pos = g_player->_pt;
		short cnt1 = 0;
		int _player_turn = 0;
		while (1)
		{
			if (dir.SetNext(pos, next_pt) != Direction::None)
				break;

			dir.Move(pos);

			cnt1++;
			if (cnt1 > 30)
				break;
		}

		pos = g_player->_pt;
		short cnt2 = 0;
		_player_turn = 1;
		while (1) 
		{
			if (dir.SetNext(pos, next_pt) != Direction::None)
				break;

			dir.Move(pos);

			cnt2++;
			if (cnt2 > 30)
				break;
		}

		if (cnt1 > cnt2)
			_player_turn = 0;
		else _player_turn = 1;

		return _player_turn;
	}(dst);

	PrintDebug("PlayerTurn!: {}", turn);

	if(turn == 0) 
	{
		for(szt i = dir._id; i <= dir._id + 2u; ++i)
		{
			if (auto ret = get(i); 
				ret != Direction::None)
			{
				return ret;
			}
		}
	}
	else if(turn == 1) 
	{
		for(szt i = dir._id; i >= dir._id - 2u; --i)
		{
			if (auto ret = get(i);
				ret != Direction::None)
			{
				return ret;
			}
		}
	}
	
	return { Direction::None };
}
}
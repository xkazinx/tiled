#pragma once

namespace common
{
namespace tile
{
	enum class Type : u8
	{
		Grass,
		Water,
		Rock,
		Max
	};

	using LineSize = u8;
	static constexpr float kSize = 32.f;
	static constexpr float kHalfSize = kSize / 2.f;

#ifdef CLIENT
	using ColorArray = const array<sf::Color, (szt)Type::Max>;
	extern ColorArray kColors;
#endif

}
}

using namespace common;
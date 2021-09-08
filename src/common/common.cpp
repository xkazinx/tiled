#include "stdafx.h"

namespace common
{
namespace tile
{
#ifdef CLIENT
	ColorArray kColors =
	{
		sf::Color::Green,
		sf::Color::Blue,
		sf::Color{ 50, 50, 50, 255 }
	};
#endif
}
}
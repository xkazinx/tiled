#include "stdafx.h"
#include "global.h"
#include "Random.h"

namespace global
{
	App* g_app;
	unique_ptr<Random> g_rand;

	namespace app
	{
		void Initialize() {
			// unordered
			g_rand = make_unique<Random>();

			// ordered
		}

		void Deinitialize() {

		}
	}
}

namespace font {
	FontNames kNames = {
		"arial.ttf",
	};
}

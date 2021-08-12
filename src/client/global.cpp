#include "stdafx.h"
#include "Global.h"
#include "Random.h"

namespace Global
{
	App* g_app;
	unique_ptr<Random> g_rand;
	
	void Initialize() {
		// unordered
		g_rand = make_unique<Random>();

		// ordered
	}

	void Deinitialize() {

	}
}

namespace font {
	FontNames kNames = {
		"arial.ttf",
	};
}

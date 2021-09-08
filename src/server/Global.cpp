#include "stdafx.h"
#include "Global.h"
#include <Socket.h>

namespace Global
{
	App* g_app;
	unique_ptr<Socket> g_sock;

	void Initialize() {
		// unordered

		g_sock = make_unique<Socket>(Socket::Type::Server, 256);

		// ordered
	}

	void Deinitialize() {

	}
}
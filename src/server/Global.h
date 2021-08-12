#pragma once

class App;

namespace Global
{
	constexpr char kAppName[] = "Tileship Server";

	extern App* g_app;

	extern void Initialize();
	extern void Deinitialize();
}
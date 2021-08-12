#include "stdafx.h"
#include "App.h"

App::App()
{
	g_app = this;
	Global::Initialize();
}

App::~App()
{
	Global::Deinitialize();
}


int App::Tick() 
{
	return 0;
}
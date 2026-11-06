#pragma once
#include <scene/manager.h>

namespace Client
{
// A scene (kazilib scene/manager.h): its frame (Tick, Render), its events
// (HandleEvent: an sf::Event) and its ImGui windows (ProcessUI, after Render)
struct Scene : lib::scene::Base
{
	Scene() {}
	virtual ~Scene() {}

	void HandleEvent(const void * event) override
	{
		if (event)
			HandleEvents(*static_cast<const sf::Event *>(event));
	}

	virtual bool HandleEvents(const sf::Event & ev) = 0;
	virtual void ProcessUI() {}
};
}

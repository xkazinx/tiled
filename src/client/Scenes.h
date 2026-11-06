#pragma once
#include "Scene.h"

namespace Client
{
// The scenes, one at a time (kazilib's lib::scene::Manager switches them): Set
// makes the scene of that id, enters it and drops the one it leaves
struct Scenes
{
	using ID = SceneID;

	Scenes();
	~Scenes();

	unique_ptr<Scene> _ptr;
	lib::scene::Manager _mgr;

	void Set(ID id);
	void Render();
	void Tick();
	void HandleEvent(const sf::Event * ev);

	// The current scene when it is a T, else null
	template <typename T>
	T * Get()
	{
		return dynamic_cast<T *>(_ptr.get());
	}
};
}

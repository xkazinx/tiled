#pragma once

namespace Client
{
class Scene;

class Scenes
{
public:
	using ID = SceneID;

	Scenes();
	~Scenes();
	
	unique_ptr<Scene> _ptr;

	void Set(ID id);
	void Render(sf::RenderTarget& tgt, const sf::RenderStates& states) const;
	void Tick();
	bool HandleEvent(const sf::Event& ev);

	template<typename T>
	T * Get()
	{
		return (T*)_ptr.get();
	}
};
}
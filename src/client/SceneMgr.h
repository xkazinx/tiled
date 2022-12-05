#pragma once

class Scene;

class SceneMgr
{
public:
	using ID = SceneID;

	SceneMgr();
	~SceneMgr();
	
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
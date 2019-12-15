#pragma once

class Scene;
class SceneMgr : public sf::Drawable
{
public:
	using ID = scene::ID;

	SceneMgr();
	~SceneMgr();

	array<unique_ptr<Scene>, ncast(ID::Max)> _list;

	auto & At(const ID & id)		{ return _list[ncast(id)]; }
	auto & At(const ID & id)  const { return _list[ncast(id)]; }

	void Set(const ID & id);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	void Update(const sf::Time & delta);
	void HandleEvent(const sf::Event & event);

	Scene * _current;
};


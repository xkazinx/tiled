#pragma once

class Scene;

class Camera : public sf::View, public sf::Drawable
{
private:
	const Scene * _scene;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
	Camera() : sf::View({ g_res_x / 2.f, g_res_y / 2.f }, { (float)g_res_x, (float)g_res_y }) {
		//setViewport({ .5,.5,1,1 });
	}
	~Camera();
	
	void BindScene(const Scene * scene) {
		_scene = scene;
	}

	auto GetLeft() const {
		return getCenter().x - getSize().x / 2.f;
	}

	auto GetRight() const {
		return getCenter().x + getSize().x / 2.f;
	}

	auto GetTop() const {
		return getCenter().y - getSize().y / 2.f;
	}

	auto GetBottom() const {
		return getCenter().y + getSize().y / 2.f;
	}
};


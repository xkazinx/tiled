#include "stdafx.h"
#include "Camera.h"
#include "Scene.h"

void Camera::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
#ifdef DEBUG_MODE
	RectangleShape rectangle;
	rectangle.setFillColor(sf::Color::Black);
	rectangle.setSize(getSize());
	target.draw(rectangle);
#endif

	target.draw(*_scene);
}

Camera::~Camera()
{
}

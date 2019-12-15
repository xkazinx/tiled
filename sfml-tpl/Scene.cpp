#include "stdafx.h"
#include "Scene.h"
#include "UIComponent.h"
#include "Button.h"
#include "Camera.h"

void Scene::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.setView(target.getDefaultView());
	for (auto & component : _ui_components) {
		target.draw(*component, states);
	}

	target.setView(*g_camera.get());

	Render({ target, states });
}

Scene::Scene(const ID & id) : _id(id)
{
}

Scene::~Scene()
{
}

void Scene::HandleEvents(const sf::Event & event)
{
	for (auto & component : _ui_components) {
		if (!component->HandleEvent(event))
			return;
	}

	HandleEvent(event);
}


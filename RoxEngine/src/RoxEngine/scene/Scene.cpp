#include "Scene.hpp"

#include <iostream>
#include <flecs/flecs.h>


namespace RoxEngine {
	Entity Scene::CreateEntity()
	{
		Entity e(mWorld.entity(), *this);
		return e;
	}

	Entity::Entity(flecs::entity id, Scene& scene)
		: mId(id)
		, mScene(scene)
	{

	}
}

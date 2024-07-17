#pragma once
#include <unordered_map>
#include <flecs/flecs.h>

namespace RoxEngine {
	class Entity;

	class Scene {
	public:
		Scene() = default;

		Entity CreateEntity();
	private:
		friend class Entity;
		flecs::world mWorld;
	};
	class Entity
	{
	public:
		void IsAlive();
		template<typename T>
		bool HasComponent()
		{
			return mId.has<T>();
		}
		template<typename T>
		T& GetComponent()
		{
			return *mId.get_mut<T>();
		}
		template<typename T>
		T& AddComponent(T&& c)
		{
			mId.set<T>(c);
			return GetComponent<T>();
		}
		template<typename T>
		void RemoveComponent()
		{
			mId.remove<T>();
		}
	private:
		friend class Scene;

		Entity(flecs::entity id, Scene& scene);

		flecs::entity mId;
		Scene& mScene;
	};
}

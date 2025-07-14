#include "RoxEngine/core/Errors.hpp"
#include "RoxEngine/core/Panic.hpp"
#include "RoxEngine/utils/Utils.hpp"
#define FLECS_CUSTOM_BUILD
#define FLECS_CPP
#include "flecs.h"
#include <memory>

namespace RoxEngine
{
	ecs_entity_t create_component_id(ecs_world_t* world, auto name, auto size, auto alignment)
	{
		ecs_entity_t id = 0;
		{
			ecs_component_desc_t desc = { 0 };
			ecs_entity_desc_t edesc = { 0 };
			edesc.id = id;
			edesc.use_low_id = true;
			edesc.symbol = name;
			desc.entity = ecs_entity_init(world, &edesc);
			desc.type.size = static_cast<ecs_size_t>(size);
			desc.type.alignment = static_cast<ecs_size_t>(alignment);
			id = ecs_component_init(world, &desc);
		}
		return id;
	}

	template<typename T>
	class ComponentRegistry
	{
	public:
		inline static const char* name = typeid(T).name();
		inline static auto size = sizeof(T);
		inline static auto alignment = alignof(T);

		static ecs_entity_t id(ecs_world_t* world)
		{
			ecs_entity_t id = ecs_lookup_symbol(world, name,true, false);
			if(id == 0)
			{
				id = create_component_id(world, name, size, alignment);
				flecs::_::register_lifecycle_actions<T>(world, id);
			}
			return id;
		}
	};
	class Entity;
	class Scene {
	public:
		Scene() : mWorld(ecs_init()){
			mData.world = mWorld;
			mData.self = this;
		}

		Entity CreateEntity();
	private:
		friend Entity;
		struct Data
		{
			std::weak_ptr<ecs_world_t> world;
			Scene* self;
		};

		Data mData;
		Ref<ecs_world_t> mWorld;
	};
	class Entity
	{
	public:
		bool IsAlive() const
		{
			if (mScene.world.expired())
				return false;
			return ecs_exists(mScene.self->mWorld.get(), mId);
		}
		template<typename T>
		bool HasComponent() const
		{
			if (!IsAlive()) Panic(sEntityNotAlive);
			auto world = mScene.world.lock().get();
			return mId.has(ComponentRegistry<T>::id(world));
		}
	
		template<typename T>
		T& GetComponent()
		{
			if (!IsAlive()) Panic(sEntityNotAlive);
			auto world = mScene.world.lock().get();
			auto cid = ComponentRegistry<T>::id(world);
			if (!mId.has(cid)) Panic(sGetComponentNo);
			return *static_cast<T*>(ecs_get_mut_id(world, mId, cid));
		}
#define ADD_COMPONENT_CHECK if (!IsAlive()) Panic(sEntityNotAlive);  auto world = mScene.world.lock().get(); auto cid = ComponentRegistry<T>::id(world); if (mId.has(cid)) Panic(sAddComponentTwice);
		template<typename T>
		T& AddComponent()
		{
			ADD_COMPONENT_CHECK;
			mId.add(cid);
			return GetComponent<T>();
		}
		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			ADD_COMPONENT_CHECK;
			T& dst = *static_cast<T*>(ecs_emplace_id(world, mId, cid));
			FLECS_PLACEMENT_NEW(&dst, T{ FLECS_FWD(args)... });
			ecs_modified_id(world, mId, cid);
			return dst;
		}
		template<typename T>
		T& AddComponent(T& c) {
			ADD_COMPONENT_CHECK;
			T& dst = *static_cast<T*>(ecs_emplace_id(world, mId, cid));
			dst = FLECS_MOV(c);
			ecs_modified_id(world, mId, cid);
			return dst;
		}
		template<typename T>
		T& AddComponent(T&& c) {
			ADD_COMPONENT_CHECK;
			T& dst = *static_cast<std::remove_reference_t<T>*>(ecs_emplace_id(world, mId, cid));
			dst = FLECS_MOV(c);
			ecs_modified_id(world, mId, cid);
			return dst;
		}
		template<typename T>
		T& AddComponent(const T&c) {
			ADD_COMPONENT_CHECK;
			T& dst = *static_cast<std::remove_reference_t<T>*>(ecs_emplace_id(world, mId, cid));
			dst = FLECS_MOV(c);
			ecs_modified_id(world, mId, cid);
			return dst;
		}

		template<typename T>
		void RemoveComponent()
		{
			if (!IsAlive()) Panic(sEntityNotAlive);
			auto world = mScene.world.lock().get();
			auto cid = ComponentRegistry<T>::id(world);

			if (!mId.has(cid)) Panic(sRemoveComponentNo);
			mId.remove(cid);
		}
	private:
		friend class Scene;

		Entity(ecs_entity_t id, Scene::Data& scene);

		flecs::entity mId;
		Scene::Data mScene;
	};

	Entity Scene::CreateEntity()
	{
		Entity e(ecs_new_w_id(mWorld.get(), 0), mData);
		return e;
	}

	Entity::Entity(ecs_entity_t id, Scene::Data& scene)
		: mId(scene.world.lock().get(), id)
		, mScene(scene)
	{

	}
}
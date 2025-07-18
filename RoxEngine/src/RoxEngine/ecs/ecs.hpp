#pragma once
#include "RoxEngine/utils/Utils.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>

namespace RoxEngine {
    class UntypedComponent;
    class Entity {
    public:
        bool exists();
        void destroy();
        template<typename T>
        bool hasComponent();
        bool hasComponent(UntypedComponent component);
        template<typename T, typename... Args>
        T* addComponent(Args&&... args);
        void* addComponent(UntypedComponent component);
        template<typename T>
        T* getComponent();
        void* getComponent(UntypedComponent component);
        template<typename T>
        void removeComponent();
        void removeComponent(UntypedComponent component);
    protected:
        friend class Scene;
        Entity(uint64_t id);

        uint64_t mId;
    };
    class UntypedComponent : public Entity{
    public:
        struct TypeInfo {
            size_t size = 0;
            size_t alignment = 0;
        };
        //Is zero typed component
        bool isTag() {return getTypeInfo().size == 0;}
        TypeInfo getTypeInfo();

        void destroy() = delete;
    private:
        friend class World;
        UntypedComponent(uint64_t id);
    };
    class Scene {
    public:
        Entity entity(const std::string& name="");
        bool exists();
        void destroy();
    private:
        friend class World;
        Scene(uint64_t id);
        
        uint64_t mId;
    };
    class World {
    public:
        static Scene createScene(const std::string& name="");
        static UntypedComponent lookupComponent(const char * name);
        static UntypedComponent lookupComponent(const std::string& name) {
            return lookupComponent(name.data());
        }
        //Finds or creates a component with name
        static UntypedComponent component(const char * name, UntypedComponent::TypeInfo info);
        template<typename T>
        static UntypedComponent component() {
            auto name = TypeToString<T>();
            if(auto c = lookupComponent(name); c.exists()) {
                return c;
            }
            //TODO: initiaize hooks, (deconstructor, constructor, copy, assign copy, etc...)
            UntypedComponent::TypeInfo info = {
                sizeof(T),
                alignof(T)
            };
            return component(name, info);
        }
        static void debugView();
    };
    template<typename T>
    bool Entity::hasComponent() {
        return hasComponent(World::component<T>());
    }
    template<typename T, typename... Args>
    T* Entity::addComponent(Args&&... args){
        auto c = World::component<T>();
        auto component = addComponent(c);
        return new (component) T(std::forward<Args>(args)...);
    }
    template<typename T>
    T* Entity::getComponent() {
        return (T*)getComponent(World::component<T>());
    }
    template<typename T>
    void Entity::removeComponent() {
        removeComponent(World::component<T>());
    }
}
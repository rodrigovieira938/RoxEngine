#pragma once
#include "RoxEngine/utils/Utils.hpp"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <type_traits>
#include <vector>

namespace RoxEngine {
    template<typename T>
    concept ComponentConcept =
        std::is_default_constructible_v<T> &&
        std::is_destructible_v<T> &&
        std::is_copy_constructible_v<T> &&
        std::is_move_constructible_v<T> &&
        std::is_copy_assignable_v<T> &&
        std::is_move_assignable_v<T> &&
        std::is_move_constructible_v<T> &&
        HasEqualOperator<T>;
    class UntypedComponent;
    class Entity {
    public:
        bool exists();
        void destroy();
        template<typename T> requires ComponentConcept<T>
        bool hasComponent() const;
        bool hasComponent(UntypedComponent component) const;
        template<typename T, typename... Args> requires ComponentConcept<T>
        T* addComponent(Args&&... args);
        void* addComponent(UntypedComponent component);
        template<typename T> requires ComponentConcept<T>
        T* getComponent();
        void* getComponent(UntypedComponent component);
        template<typename T> requires ComponentConcept<T>
        void removeComponent();
        void removeComponent(UntypedComponent component);
    protected:
        friend class Scene;
        friend class Query;
        Entity(uint64_t id);

        uint64_t mId;
    };
    class UntypedComponent : public Entity{
    public:
        struct TypeInfo {
            size_t size = 0;
            size_t alignment = 0;
        };
        struct Hooks {
            using xtor = void(*)(void* obj, uint32_t count, void* _);
            //Hook with 2 params
            using hook2 = void(*)(const void *src,void *dest, uint32_t count, void* _);
            using equals_hook = bool(*)(const void *lhs,const void *rhs, void* _);
            xtor ctor = nullptr, dtor = nullptr;
            hook2 move = nullptr, copy = nullptr, copy_ctor = nullptr, move_ctor = nullptr;
            equals_hook equals = nullptr;
        };
        //Is zero typed component
        bool isTag() {return getTypeInfo().size == 0;}
        TypeInfo getTypeInfo();

        void destroy() = delete;
    private:
        friend class World;
        friend class QueryBuilder;
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
        static UntypedComponent component(const char * name, UntypedComponent::TypeInfo info, UntypedComponent::Hooks hooks);
        template<typename T> requires ComponentConcept<T>
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
            UntypedComponent::Hooks hooks={};
            hooks.ctor = [](void* obj, uint32_t count, void*) {
                T* ptr = static_cast<T*>(obj);
                for (uint32_t i = 0; i < count; ++i) {
                    new (&ptr[i]) T();
                }
            };
            hooks.dtor = [](void* obj, uint32_t count, void*) {
                T* ptr = static_cast<T*>(obj);
                for (uint32_t i = 0; i < count; ++i) {
                    ptr[i].~T();
                }
            };
            hooks.copy_ctor = [](const void* src, void* dest, uint32_t count, void*) {
                const T* s = static_cast<const T*>(src);
                T* d = static_cast<T*>(dest);
                for (uint32_t i = 0; i < count; ++i) {
                    new (&d[i]) T(s[i]);
                }
            };
            hooks.move_ctor = [](const void* src, void* dest, uint32_t count, void*) {
                const T* s = static_cast<const T*>(src);
                T* d = static_cast<T*>(dest);
                for (uint32_t i = 0; i < count; ++i) {
                    new (&d[i]) T(std::move(s[i]));
                }
            };
            hooks.copy = [](const void* src, void* dest, uint32_t count, void*) {
                const T* s = static_cast<const T*>(src);
                T* d = static_cast<T*>(dest);
                for (uint32_t i = 0; i < count; ++i) {
                    d[i] = s[i];
                }
            };
            hooks.move = [](const void* src, void* dest, uint32_t count, void*) {
                const T* s = static_cast<const T*>(src);
                T* d = static_cast<T*>(dest);
                for (uint32_t i = 0; i < count; ++i) {
                    d[i] = std::move(s[i]);
                }
            };
            hooks.equals = [](const void* lhs, const void* rhs, void*) {
                const T* a = static_cast<const T*>(lhs);
                const T* b = static_cast<const T*>(rhs);
                return (*a) == (*b);
            };
            auto c = component(name, info, hooks);
            
            return c;
        }
        static void debugView();
    };
    class Query {
    public:
        void each(std::function<void(Entity e)>);
    private:
        friend class QueryBuilder;
        Query(void* query);
        void* mQuery;
    };
    class QueryBuilder {
    public:
        enum class Op {
            AND,
            OR,
            NOT
        };
        struct Term {
            Op op;
            UntypedComponent component;
            bool groupBegin = false;
            bool groupEnd = false;
        };
        QueryBuilder& with(UntypedComponent component) { terms.emplace_back(Term{Op::AND, component, false}); return *this;}
        QueryBuilder& without(UntypedComponent component) { terms.emplace_back(Term{Op::NOT, component, true}); return *this;}
        QueryBuilder& or_with(UntypedComponent component) { terms.emplace_back(Term{Op::OR, component, false}); return *this;}
        QueryBuilder& group_begin() {terms.emplace_back(Term{Op::AND, {0}, false, true});return *this;}
        QueryBuilder& group_end() {terms.emplace_back(Term{Op::AND, {0}, false, false});return *this;}
        
        template<typename T>
        QueryBuilder& with() {
            return with(World::component<T>());
        }
        template<typename T>
        QueryBuilder& without() {
            return without(World::component<T>());
        }
        template<typename T>
        QueryBuilder& or_with() {
            return or_with(World::component<T>());
        }
        Query build();
        private:
        std::vector<Term> terms;
    };
    template<typename T> requires ComponentConcept<T>
    bool Entity::hasComponent() const{
        return hasComponent(World::component<T>());
    }
    template<typename T, typename... Args> requires ComponentConcept<T>
    T* Entity::addComponent(Args&&... args){
        auto c = World::component<T>();
        auto component = addComponent(c);
        return new (component) T(std::forward<Args>(args)...);
    }
    template<typename T> requires ComponentConcept<T>
    T* Entity::getComponent() {
        return (T*)getComponent(World::component<T>());
    }
    template<typename T> requires ComponentConcept<T>
    void Entity::removeComponent() {
        removeComponent(World::component<T>());
    }
}
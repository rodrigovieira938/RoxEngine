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
    class UntypedRelation;
    template<typename T> requires ComponentConcept<T>
    class Relation;
    class Entity {
    public:
        std::string_view name();
        void name(std::string_view name);
        bool exists();
        void destroy();

        UntypedRelation addRelation(UntypedComponent tag, Entity target);
        bool hasRelation(UntypedComponent tag, Entity target) const;
        UntypedRelation getRelation(UntypedComponent tag, Entity target);
        void removeRelation(UntypedComponent tag, Entity target);

        template<typename T, typename... Args> requires ComponentConcept<T>
        Relation<T> addRelation(Entity target, Args&&... args);
        template<typename T> requires ComponentConcept<T>
        bool hasRelation(Entity target) const;
        template<typename T> requires ComponentConcept<T>
        Relation<T> getRelation(Entity target);
        template<typename T> requires ComponentConcept<T>
        void removeRelation(Entity target);

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
        friend class QueryBuilder;
        friend class UntypedRelation;
        friend class QueryIter;
        Entity(uint64_t id);

        uint64_t mId;
    };
    class UntypedComponent : public Entity {
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
        std::string_view symbol();
        void symbol(std::string_view name);

        //Is zero typed component
        bool isTag() {return getTypeInfo().size == 0;}
        TypeInfo getTypeInfo();

        void destroy() = delete;
    private:
        friend class World;
        friend class QueryBuilder;
        friend class QueryIter;
        UntypedComponent(uint64_t id);
    };
    class UntypedRelation {
    public:
        void* get() {return ptr;}
        UntypedComponent getTag() {return tag;}
        Entity getTarget() {return target;}
        bool ok();
    private:
        friend class Entity;
        template<typename T> requires ComponentConcept<T>
        friend class Relation;
        friend class QueryIter;
        UntypedComponent tag;
        Entity target;
        //Ptr to the data, if tag is 0 sized do not write to this address 
        void* ptr;
        UntypedRelation(UntypedComponent tag, Entity target, void* ptr) : tag(tag), target(target), ptr(ptr){}
    };
    template<typename T> requires ComponentConcept<T>
    class Relation : public UntypedRelation{
    public:
        T* get() {return static_cast<T*>(UntypedRelation::get());}
        T* operator->() {
            return get();
        }
    private:
        friend class Entity;
        Relation(UntypedComponent tag, Entity target, void* ptr) : UntypedRelation(tag, target, ptr){}
    };
    class Scene {
    public:
        Entity entity(const std::string& name="");
        Entity lookup(const std::string& name);
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
        static UntypedComponent lookupComponentBySymbol(const char * name);
        static UntypedComponent lookupComponentBySymbol(const std::string& name) {
            return lookupComponent(name.data());
        }
        //Finds or creates a component with name
        static UntypedComponent component(const char * name, UntypedComponent::TypeInfo info, UntypedComponent::Hooks hooks);
        template<typename T> requires ComponentConcept<T>
        static UntypedComponent component() {
            auto name = typeid(T).name();
            if(auto c = lookupComponentBySymbol(name); c.exists()) {
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
            c.symbol(name);
            return c;
        }
        static void debugView();
    };
    class Query;
    class QueryIter {
    public:
        void* get(int termIndex);
        UntypedRelation getRelation(int termIndex);
    private:
        friend Query;
        
        struct Impl;
        QueryIter(Impl* impl, Query* query) : impl(impl), query(query) {}

        //No need to delete this since its a pointer to a stack variable only valid on the each function
        Impl* impl;
        Query* query;
    };
    class Query {
    public:
        void each(std::function<void(Entity e, QueryIter& iter)>);
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
            enum class Flags {
                NONE = 0,
                GROUP_BEGIN,
                GROUP_END,
                RELATION,
            };
            Op op;
            UntypedComponent component;
            Entity target;
            Flags flags = Flags::NONE;

            Term(Op op, UntypedComponent component, Flags flags) : op(op), component(component), target(0), flags(flags) {}
            Term(Flags flags) : component(0), target(0), flags(flags) {}
            Term(Op op, UntypedComponent component) : Term(op, component, Flags::NONE) {}
            Term(Op op, UntypedComponent component, Entity target) : op(op), component(component), target(target), flags(Flags::RELATION) {}
        };
        QueryBuilder& with(UntypedComponent component) { terms.emplace_back(Term(Op::AND, component)); return *this;}
        QueryBuilder& with_relation(UntypedComponent tag, Entity target) {terms.emplace_back(Term(Op::AND, tag, target)); return *this;}
        QueryBuilder& with_relation(UntypedComponent tag) {terms.emplace_back(Term(Op::AND, tag, Term::Flags::RELATION)); return *this;}
        QueryBuilder& without(UntypedComponent component) { terms.emplace_back(Term(Op::NOT, component)); return *this;}
        QueryBuilder& without_relation(UntypedComponent tag, Entity target) {terms.emplace_back(Term(Op::NOT, tag, target)); return *this;}
        QueryBuilder& without_relation(UntypedComponent tag) {terms.emplace_back(Term(Op::NOT, tag, Term::Flags::RELATION)); return *this;}
        QueryBuilder& or_with(UntypedComponent component) { terms.emplace_back(Term(Op::OR, component)); return *this;}
        QueryBuilder& or_with_relation(UntypedComponent tag, Entity target) {terms.emplace_back(Term(Op::OR, tag, target)); return *this;}
        QueryBuilder& group_begin() {terms.emplace_back(Term(Term::Flags::GROUP_BEGIN));return *this;}
        QueryBuilder& group_end() {terms.emplace_back(Term(Term::Flags::GROUP_END));return *this;}
        
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
        template<typename T>
        QueryBuilder& with_relation(Entity target) {
            return with_relation(World::component<T>(), target);
        }
        template<typename T>
        QueryBuilder& with_relation() {
            return with_relation(World::component<T>());
        }
        template<typename T>
        QueryBuilder& without_relation(Entity target) {
            return without_relation(World::component<T>(), target);
        }
        template<typename T>
        QueryBuilder& without_relation() {
            return without_relation(World::component<T>());
        }
        template<typename T>
        QueryBuilder& or_with_relation(Entity target) {
            return or_with_relation(World::component<T>(), target);
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

    template<typename T, typename... Args> requires ComponentConcept<T>
    Relation<T> Entity::addRelation(Entity target, Args&&... args) {
        UntypedRelation untyped = addRelation(World::component<T>(), target);
        new (untyped.ptr) T(std::forward<Args>(args)...);
        return Relation<T>(untyped.tag, untyped.target, untyped.ptr);
    }
    template<typename T> requires ComponentConcept<T>
    bool Entity::hasRelation(Entity target) const{
        UntypedComponent component = World::component<T>(); 
        return hasRelation(component, target);
    }
    template<typename T> requires ComponentConcept<T>
    Relation<T> Entity::getRelation(Entity target) {
        UntypedRelation untyped = getRelation(World::component<T>(), target);
        return Relation<T>(untyped.tag, untyped.target, untyped.ptr);
    }
    template<typename T> requires ComponentConcept<T>
    void Entity::removeRelation(Entity target) {
        removeRelation(World::component<T>(), target);
    }
}
#pragma once
#include "RoxEngine/utils/Utils.hpp"
#include <cstddef>
#include <cstdint>
#include <string>

namespace RoxEngine {
    class Entity {
    public:
        bool exists();
        void destroy();
    protected:
        friend class Scene;
        Entity(uint64_t id);

        uint64_t mId;
    };
    class UntypedComponent : public Entity{
    public:
        //Is zero typed component
        bool isTag() {return getSize() == 0;}
        size_t getSize();
        size_t getAlignment();

        void destroy() = delete;
    private:
        friend class World;
        UntypedComponent(uint64_t id);
    };
    class Scene {
    public:
        Entity entity();
        bool exists();
        void destroy();
    private:
        friend class World;
        Scene(uint64_t id);
        
        uint64_t mId;
    };
    class World {
    public:
        static Scene createScene();
        static UntypedComponent lookupComponent(const char * name);
        static UntypedComponent lookupComponent(const std::string& name) {
            return lookupComponent(name.data());
        }
        //Finds or creates a component with name
        static UntypedComponent component(const char * name);
        template<typename T>
        static UntypedComponent component() {
            return component(TypeToString<T>());
        }
        static void debugView();
    };
}
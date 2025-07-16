#pragma once
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
        static UntypedComponent lookupComponent(const std::string& name);
        static void debugView();
    };
}
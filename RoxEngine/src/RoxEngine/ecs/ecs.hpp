#pragma once

#include <cstdint>
namespace RoxEngine {
    class Entity {
    private:
        friend class Scene;
        Entity(uint64_t id);

        uint64_t mId;
    };
    class Scene {
    public:
        Entity entity();
    private:
        friend class World;
        Scene(uint64_t id);
        
        uint64_t mId;
    };
    class World {
    public:
        static Scene createScene();
        static void debugView();
    };
}
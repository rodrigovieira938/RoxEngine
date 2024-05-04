#pragma once
#include <RoxEngine/core/Window.hpp>
#include <RoxEngine/utils/Memory.h>

namespace RoxEngine {
    class Engine {
    public:
        static Engine* Get();
        //TODO: Make this return an error
        int Run();
    private:
        Ref<Window> mWindow;

        Engine() = default;
    };
}
#pragma once
#include <RoxEngine/core/Window.hpp>
#include <RoxEngine/utils/Memory.h>

namespace RoxEngine {
    class Engine {
    public:
        static Engine* Get();
        //TODO: Make this return an error
        int Run();
        inline Ref<Window> GetWindow() {return mWindow;};
    private:
        Ref<Window> mWindow;

        void DrawDebugInfo();

        Engine() = default;
    };
}
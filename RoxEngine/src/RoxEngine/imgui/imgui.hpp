#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>

namespace RoxEngine {
    class ImGuiLayer {
    private:
        friend class Engine;

        static void Init();
        static void NewFrame();
        static void Render();
        static void Shutdown();        
    };
}
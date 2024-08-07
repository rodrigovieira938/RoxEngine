module;
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
export module roxengine:imgui;
export namespace RoxEngine {
    class ImGuiLayer {
    private:
        friend class Engine;

        static void Init();
        static void NewFrame();
        static void Render();
        static void Shutdown();        
    };
}
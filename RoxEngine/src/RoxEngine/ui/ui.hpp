#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
namespace RoxEngine {
    class Engine;
    namespace UI {
        class Layer {
        private:
            friend Engine;

            static void Init();
            static void NewFrame();
            static void Render();
            static void Shutdown();
        };
        #define EMPTY_FUNC(name, return_value) \
            template<typename... Args> \
            auto name(Args&&...) {return return_value;}
        EMPTY_FUNC(TreeNodeEx, false);
        EMPTY_FUNC(TreePop, false);
        EMPTY_FUNC(IsItemHovered, false);
        EMPTY_FUNC(IsMouseClicked, false);
        EMPTY_FUNC(Begin, false);
        EMPTY_FUNC(End, false);
        EMPTY_FUNC(Text, false);
        EMPTY_FUNC(SameLine, false);
        EMPTY_FUNC(SliderFloat3, false);
        EMPTY_FUNC(SliderFloat, false);
        EMPTY_FUNC(DragFloat3, false);
        #define CONSTEXPR(name, value) static constexpr auto name = value;
        CONSTEXPR(ImGuiTreeNodeFlags_Selected, 0);
        CONSTEXPR(ImGuiTreeNodeFlags_OpenOnArrow, 0);
        CONSTEXPR(ImGuiTreeNodeFlags_OpenOnDoubleClick, 0);
        CONSTEXPR(ImGuiMouseButton_Left, 0);
    }
}
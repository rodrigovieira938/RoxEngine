#pragma once
#include <cstdint>
#include <limits>
#include <string_view>
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

        namespace ElementSizing {
            enum Type {
                FIT,
                FIXED,
                GROW
            };
            struct Value {
                Type type = Type::FIT;
                uint32_t value = 0;

                operator decltype(value)() const { return value; }
                operator decltype(value)&() { return value; }
            };

            inline constexpr static Value fit() {return {FIT, 0};}
            inline constexpr static Value grow() {return {GROW, 0};}
            inline constexpr static Value fixed(uint32_t value) {return {FIXED, value};}
        };
        
        enum class FlowDirection {
            LEFT_TO_RIGHT,
            RIGHT_TO_LEFT,
            TOP_TO_BOTTOM,
            BOTTOM_TO_TOP
        };
        struct Element {
            struct {
                ElementSizing::Value width;
                ElementSizing::Value height;
                uint32_t min_width = 0;
                uint32_t max_width = std::numeric_limits<decltype(max_width)>::max();
                uint32_t min_height = 0;
                uint32_t max_height = std::numeric_limits<decltype(max_height)>::max();
            } sizing;
            FlowDirection flowDirection = FlowDirection::LEFT_TO_RIGHT;
            struct {
                uint32_t left = 0, right = 0;
                uint32_t top = 0, bottom = 0;
            } padding;
            uint32_t childGap = 0;
        };
        void BeginElement(Element);
        void EndElement();
        void TextElement(std::string_view str);
        
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
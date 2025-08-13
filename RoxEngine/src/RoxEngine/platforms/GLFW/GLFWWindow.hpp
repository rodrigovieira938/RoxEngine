#pragma once
#include <string>
#include <RoxEngine/core/Window.hpp>
namespace RoxEngine::GLFW {
    enum class RendererBackend {
        AUTO = 0,
        OPENGL = 1,
    };
    class Window final : public ::RoxEngine::Window {
    public:
        //TODO: Add args to constructor
        Window(RendererBackend backend = RendererBackend::AUTO);
        ~Window();
        bool IsOpen();
        void PollEvents();
        void SetTitle(const std::string& title);
        std::string GetTitle();
        void SetSize(int width, int height);
        std::pair<int,int> GetSize();
        void SetFullscren(bool val);
        bool IsFullscreen();
        void SetMaximized(bool val);
        bool IsMaximized();
        Ref<alina::IDevice> GetDevice() {return mDevice;}

        Ref<alina::IDevice> mDevice;
        void* mWindow;
    };
}
#include "Engine.hpp"
#include "RoxEngine/renderer/GraphicsContext.hpp"
#include <GLFW/glfw3.h>
#include <RoxEngine/platforms/GLFW/GLFWWindow.hpp>

namespace RoxEngine {
    static Engine* sEngine = nullptr;
    Engine* Engine::Get() {
        if(!sEngine)
            sEngine = new Engine();
        return sEngine;
    }
    int Engine::Run() {
        if(!glfwInit()) return 1;
        mWindow = CreateRef<GLFW::Window>();
        GraphicsContext::Init(RendererApi::OPENGL);

        GraphicsContext::ClearColor(100.0/255.0,149.0/255.0,237.0/255.0);

        while(mWindow->IsOpen()) {
            mWindow->PollEvents();
            GraphicsContext::ClearScreen();
            glfwSwapBuffers((GLFWwindow*)((GLFW::Window*)mWindow.get())->mWindow);
        }
        GraphicsContext::Shutdown();
        glfwTerminate();
        delete sEngine;
        return 0; // OK
    }
}
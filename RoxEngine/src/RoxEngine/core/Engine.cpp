#include "Engine.hpp"
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
        while(mWindow->IsOpen()) {
            mWindow->PollEvents();
            glfwSwapBuffers((GLFWwindow*)((GLFW::Window*)mWindow.get())->mWindow);
        }
        glfwTerminate();
        delete sEngine;
        return 0; // OK
    }
}
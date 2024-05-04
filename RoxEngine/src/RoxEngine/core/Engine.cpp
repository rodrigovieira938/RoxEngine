#include "Engine.hpp"
#include "RoxEngine/profiler/cpu_profiler.hpp"
#include "RoxEngine/renderer/GraphicsContext.hpp"
#include <GLFW/glfw3.h>
#include <RoxEngine/platforms/GLFW/GLFWWindow.hpp>
#include <RoxEngine/profiler/profiler.hpp>
#include <iostream>

namespace RoxEngine {
    static Engine* sEngine = nullptr;
    Engine* Engine::Get() {
        if(!sEngine)
            sEngine = new Engine();
        return sEngine;
    }
    int Engine::Run() {
        PROFILER_BEGIN_SESSION("RoxEngine");
        if(!glfwInit()) return 1;
        mWindow = CreateRef<GLFW::Window>();
        GraphicsContext::Init(RendererApi::OPENGL);

        GraphicsContext::ClearColor(100.0/255.0,149.0/255.0,237.0/255.0);

        while(mWindow->IsOpen()) {
            PROFILER_SCOPE("Frame");
            mWindow->PollEvents();
            GraphicsContext::ClearScreen();
            glfwSwapBuffers((GLFWwindow*)((GLFW::Window*)mWindow.get())->mWindow);
        }
        GraphicsContext::Shutdown();
        glfwTerminate();
        delete sEngine;
        PROFILER_END_SESSION();
        return 0; // OK
    }
}
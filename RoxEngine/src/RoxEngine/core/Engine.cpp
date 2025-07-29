#include "GLFW/glfw3.h"
#include "RoxEngine/profiler/profiler.hpp"
#include <RoxEngine/core/Engine.hpp>
#include <RoxEngine/input/Input.hpp>
#include <RoxEngine/platforms/GLFW/GLFWWindow.hpp>
#include <RoxEngine/imgui/imgui.hpp>
namespace RoxEngine {
    static Engine* sEngine = nullptr;
    Engine* Engine::Get() {
        if(!sEngine)
            sEngine = new Engine();
        return sEngine;
    }
    int Engine::Run(Scope<Game> game) {
        //TODO: add the option to change the folder at initialization not runtime
        PROFILER_BEGIN_SESSION("RoxEngine");
        if(!glfwInit()) return 1;
        mWindow = CreateRef<GLFW::Window>();
        ImGuiLayer::Init();
        Input::Init();
        game->Init();
        while(mWindow->IsOpen()) {
            PROFILER_SCOPE("Frame");
            Input::Update();
            mWindow->PollEvents();
            game->Update();
            ImGuiLayer::NewFrame();
            game->Render();
            DrawDebugInfo();
            ImGuiLayer::Render();
            glfwSwapBuffers(static_cast<GLFWwindow*>(std::static_pointer_cast<GLFW::Window>(mWindow)->mWindow));
        }
        game.reset();
        Input::Shutdown();
        ImGuiLayer::Shutdown();
        glfwTerminate();
        delete sEngine;
        PROFILER_END_SESSION();
        return 0; // OK
    }
    void Engine::DrawDebugInfo() {
    }
}

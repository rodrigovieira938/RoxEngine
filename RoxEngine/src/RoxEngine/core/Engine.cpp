#include "Engine.hpp"
#include "RoxEngine/imgui/imgui.hpp"
#include "RoxEngine/renderer/GraphicsContext.hpp"
#include "imgui.h"
#include <RoxEngine/profiler/profiler.hpp>
#include <GLFW/glfw3.h>
#include <RoxEngine/platforms/GLFW/GLFWWindow.hpp>
#include <RoxEngine/input/Input.hpp>
#include <iostream>
#include "RoxEngine/renderer/Shader.hpp"

namespace RoxEngine {
    static Engine* sEngine = nullptr;
    Engine* Engine::Get() {
        if(!sEngine)
            sEngine = new Engine();
        return sEngine;
    }
    int Engine::Run(Scope<Game> game) {
        PROFILER_BEGIN_SESSION("RoxEngine");
        if(!glfwInit()) return 1;
        mWindow = CreateRef<GLFW::Window>();
        GraphicsContext::Init(RendererApi::OPENGL);
        GraphicsContext::ClearColor(100.0f/255.0f,149.0f/255.0f,237.0f/255.0f);
        ImGuiLayer::Init();
        Input::Init();
        game->Init();
        while(mWindow->IsOpen()) {
            PROFILER_SCOPE("Frame");
            Input::Update();
            mWindow->PollEvents();
            game->Update();
            game->Render();
            ImGuiLayer::NewFrame();
            DrawDebugInfo();
            ImGuiLayer::Render();
            glfwSwapBuffers(static_cast<GLFWwindow*>(std::static_pointer_cast<GLFW::Window>(mWindow)->mWindow));
        }
        game.reset();
        Input::Shutdown();
        ImGuiLayer::Shutdown();
        GraphicsContext::Shutdown();
        glfwTerminate();
        delete sEngine;
        PROFILER_END_SESSION();
        return 0; // OK
    }
    void Engine::DrawDebugInfo() {
        if(ImGui::Begin("Window 1")) {
        }
        ImGui::End();
        if(ImGui::Begin("Window 2")) {
        }
        ImGui::End();
    }
}
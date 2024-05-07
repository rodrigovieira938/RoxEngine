#include "Engine.hpp"
#include "RoxEngine/imgui/imgui.hpp"
#include "RoxEngine/renderer/GraphicsContext.hpp"
#include "imgui.h"
#include <RoxEngine/profiler/profiler.hpp>
#include <GLFW/glfw3.h>
#include <RoxEngine/platforms/GLFW/GLFWWindow.hpp>
#include <RoxEngine/input/Input.hpp>
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
        ImGuiLayer::Init();
        Input::Init();
        auto va = VertexArray::Create();
        {
            float vertices[] = {
                -.5,-.5,.0,
                -.5, .5,.0,
                .5, .5,.0,
                .5,-.5,.0,
            };
            uint32_t indices[] = {
                0,2,1,
                0,3,2
            };

            auto vb = VertexBuffer::Create(vertices, sizeof(float)*3*4);
            vb->SetLayout({{"mPos", ShaderDataType::Float3}});
            auto ib = IndexBuffer::Create(indices, 6);

            va->AddVertexBuffer(vb);
            va->SetIndexBuffer(ib);
        }

        while(mWindow->IsOpen()) {
            PROFILER_SCOPE("Frame");
            Input::Update();
            mWindow->PollEvents();
            if(Input::GetKeyState(Key::W) != KeyState::NONE)
                std::cout << "W KEY action: " << (int)Input::GetKeyState(Key::W) << std::endl;

            ImGuiLayer::NewFrame();
            GraphicsContext::ClearScreen();
            GraphicsContext::Draw(va, va->GetIndexBuffer()->GetCount());
            DrawDebugInfo();
            ImGuiLayer::Render();
            glfwSwapBuffers((GLFWwindow*)((GLFW::Window*)mWindow.get())->mWindow);
        }
        va.reset();
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
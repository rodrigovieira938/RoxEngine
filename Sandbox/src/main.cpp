#include "RoxEngine/input/Input.hpp"
#include "RoxEngine/renderer/Buffer.hpp"
#include "RoxEngine/renderer/Shader.hpp"
#include "RoxEngine/renderer/VertexArray.hpp"
#include "RoxEngine/utils/Memory.h"
#include <RoxEngine/core/Engine.hpp>
#include <RoxEngine/renderer/GraphicsContext.hpp>
#include <iostream>
#include <filesystem>
#include <slang/slang.h>
#include <slang/slang-com-ptr.h>

using namespace RoxEngine;

struct TestGame final : public Game {
    Ref<VertexArray> va;
    Ref<Shader> shader;
    Ref<Framebuffer> fb;
    ~TestGame() override { std::cout << "GOODBYE!\n"; }

    void Init() override {
        std::cout << "HelloWorld!\n";

    	va = VertexArray::Create();
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
        fb = Framebuffer::Create(800, 800, {FramebufferColorTexFormat::RGB32}, FramebufferDepthTexFormat::D24UNS8U);
        shader = Shader::Create(R"(#version 330 core
            layout (location = 0) in vec3 aPos;
            
            void main()
            {
                gl_Position = vec4(aPos, 1.0); // see how we directly give a vec3 to vec4's constructor
            }
        )",R"(#version 330 core
            out vec4 FragColor;

            void main()
            {
                FragColor = vec4(1,0,0,1);
            } 
        )");
    }
    void Update() override {
        if(Input::GetKeyState(Key::W) != KeyState::NONE)
                std::cout << "W KEY action: " << static_cast<int>(Input::GetKeyState(Key::W)) << "\n";
    }
    void Render() override {
        GraphicsContext::ClearScreen();
        GraphicsContext::UseShader(shader);
        GraphicsContext::Draw(va, va->GetIndexBuffer()->GetCount());
    }
};

using namespace RoxEngine;
int main(int, char**) {
    return Engine::Get()->Run(CreateScope<TestGame>());
}

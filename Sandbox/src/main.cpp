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
#include "RoxEngine/filesystem/Filesystem.hpp"
#include <RoxEngine/scene/Scene.hpp>
using namespace RoxEngine;

struct TestGame final : public Game {
    Ref<VertexArray> va;
    Ref<Shader> shader;
    Ref<Framebuffer> fb;

    struct TestComponent { std::string a = "First!"; };

    void Init() override {
    	Scene s;
        Entity e = s.CreateEntity();
        std::cout << std::boolalpha << e.HasComponent<TestComponent>() << "\n";
        e.AddComponent<TestComponent>({});
        std::cout << std::boolalpha << e.HasComponent<TestComponent>() << "\n";
        std::cout << e.GetComponent<TestComponent>().a << "\n";
        e.GetComponent<TestComponent>().a = "Second!";
    	std::cout << e.GetComponent<TestComponent>().a << "\n";
        e.RemoveComponent<TestComponent>();
        std::cout << e.AddComponent<TestComponent>({ "Third!" }).a << "\n";
        e.RemoveComponent<TestComponent>();
    	std::cout << std::boolalpha << e.HasComponent<TestComponent>() << "\n";

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
        shader = Shader::Create("res://shaders/basic.slang");
        if(FileSystem::Exists("res://test.txt"))
        {
			std::string content = FileSystem::ReadTextFile("res://test.txt");
			std::cout << "res://test.txt contents = \"" << content << "\"\n";
        }
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

Scope<Game> CreateGame()
{
    return CreateScope<TestGame>();
}

//todo: move constructor to the engine project
int main(int, char**) {
    return Engine::Get()->Run(CreateGame());
}

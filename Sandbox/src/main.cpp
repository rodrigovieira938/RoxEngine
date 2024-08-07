#include <iostream>
#include <filesystem>

#include <glm/glm.hpp>

import roxengine;
using namespace RoxEngine;

struct TestGame final : public Game {
    Ref<VertexArray> va;
    Ref<Shader> shader;
    Ref<Material> material;
    Ref<Framebuffer> fb;

    struct TestComponent { std::string a = "First!"; };

    void Init() override {
        // Asserts because of c++ wrapper (internal linkage of static variables) TODO: make own c++ wrapper
    	/*Scene s;
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
		*/

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
        material = Material::Create(shader, Material::EntryPointInfo{ "basic_vmain", "basic_fmain" });
        if(FileSystem::Exists("res://test.txt"))
        {
			std::string content = FileSystem::ReadTextFile("res://test.txt");
			std::cout << "res://test.txt contents = \"" << content << "\"\n";
        }

        if(auto ubo = material->GetUbo("Uniforms"))
        {
            glm::vec3 color = { 255.f / 255.f,223.f / 255.f,214.f / 255.f, };
            glm::mat4 matrix = glm::mat4(1.f);

            ubo->Set("color", &color.x, sizeof(glm::vec3));
            ubo->Set("matrix", &matrix[0][0], sizeof(glm::mat4));
        }
    }
    void Update() override {
        if(Input::GetKeyState(Key::W) != KeyState::NONE)
                std::cout << "W KEY action: " << static_cast<int>(Input::GetKeyState(Key::W)) << "\n";
    }
    void Render() override {
        GraphicsContext::ClearScreen();
        GraphicsContext::UseMaterial(material);
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
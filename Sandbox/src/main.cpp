#include <RoxEngine/RoxEngine.hpp>
#include <RoxEngine/renderer/VertexArray.hpp>
#include <RoxEngine/renderer/Material.hpp>
#include <RoxEngine/renderer/GraphicsContext.hpp>
#include <glm/glm.hpp>

using namespace RoxEngine;

struct TestGame final : public Game {
    Ref<VertexArray> va;
    Ref<Shader> shader;
    Ref<Material> material;
    Ref<Framebuffer> fb;

    struct TestComponent
    {
        TestComponent(const std::string& s) { a = s; log::info("TestComponent::TestComponent(const std::string&)");}
        TestComponent(const TestComponent& other) { a = std::move(other.a); log::info("TestComponent::TestComponent(TestComponent&)"); }
        ~TestComponent() { log::info("TestComponent::~TestComponent()"); }
        TestComponent& operator=(const TestComponent& other) { a = std::move(other.a);  log::info("TestComponent::operator=(const TestComponent&)"); return *this; }
	    std::string a = "<OOPS>";
    };
     
    void Init() override {
        Scene s = World::createScene();
        Entity e = s.entity();
        World::component<TestComponent>();
    	/*log::info(e.HasComponent<TestComponent>());
        e.AddComponent<TestComponent>("First");
        log::info(e.HasComponent<TestComponent>());
        log::info(e.GetComponent<TestComponent>().a);
        e.GetComponent<TestComponent>().a = "Second!";
    	log::info(e.GetComponent<TestComponent>().a);
        e.RemoveComponent<TestComponent>();
        log::info(e.AddComponent<TestComponent>({ "Third!" }).a);
        e.RemoveComponent<TestComponent>();
        log::info(e.HasComponent<TestComponent>());*/
		

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

            AssetManager::TextAssetDecoder txtDecoder;
            AssetManager::Load<AssetManager::TextAssetDecoder>("res://test.txt", txtDecoder);

        	log::info("res://test.txt name=\"{}\" extension=\"{}\" contents = \"{}\"",
                FileSystem::GetFileName("res://test.txt", false), 
                FileSystem::GetFileExtension("res://test.txt"), 
                txtDecoder.content);
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
               log::info("W KEY action: {}",static_cast<int>(Input::GetKeyState(Key::W)));
    }
    void Render() override {
        GraphicsContext::ClearScreen();
        World::debugView();
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
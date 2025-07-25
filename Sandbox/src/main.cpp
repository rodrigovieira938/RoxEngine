#include "RoxEngine/ecs/ecs.hpp"
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
        TestComponent() {};
        TestComponent(const std::string& s) { a = s;}
        ~TestComponent() {log::info("~TestComponent()");}
	    bool operator ==(const TestComponent& other) const {return a == other.a;}
        std::string a = "<OOPS>";
    };
    struct ComponentA {
	    bool operator ==(const ComponentA& other) const {return true;}
    };
    struct ComponentB {
	    bool operator ==(const ComponentB& other) const {return true;}
    };
    struct ComponentC {
	    bool operator ==(const ComponentC& other) const {return true;}
    };
    struct ComponentD {
	    bool operator ==(const ComponentD& other) const {return true;}
    };
    struct ConvertsTo {
        float factor;
	    bool operator ==(const ConvertsTo& other) const {return factor == other.factor;}
    };
    void TestQuery(Scene& scene) {
        Entity e1 = scene.entity("Entity1");
        Entity e2 = scene.entity("Entity2");
        Entity e3 = scene.entity("Entity3");
        Entity e4 = scene.entity("Entity4");
        Entity e5 = scene.entity("Entity5");
        e1.addComponent<ComponentA>();
        e1.addComponent<ComponentB>();
        e2.addComponent<ComponentA>();
        e2.addComponent<ComponentC>();
        e3.addComponent<ComponentA>();
        e3.addComponent<ComponentD>();
        e4.addComponent<ComponentC>();
        e5.addComponent<ComponentA>();
        e5.addComponent<ComponentB>();
        e5.addComponent<ComponentC>();
        e5.addComponent<ComponentD>();

        auto query = QueryBuilder()
            .with<ComponentA>()
            .without<ComponentB>()
            .or_with<ComponentC>()
            .with<ComponentD>()
            .build();
        query.each([](Entity e){
            //TODO: print name of the entity
            log::info("Match {}", e.name());
        });
    }
    void TestRelations(Scene& scene) {
        auto meter = World::component("meter", {}, {});
        auto centimeter = World::component("centimeter", {}, {});
        centimeter.addRelation<ConvertsTo>(meter, 1.f/100.f);
        meter.addRelation<ConvertsTo>(centimeter, 100.f);

        auto convert = [](float value, UntypedComponent from, UntypedComponent to){
            if(!from.hasRelation<ConvertsTo>(to)) {
                log::error("Cannot convert from {} to {}", from.name(), to.name());
                return 0.f;
            }
            return value * from.getRelation<ConvertsTo>(to)->factor;
        };
        log::info("Converting 100cm to m = {}", convert(100, centimeter, meter));
        centimeter.removeRelation<ConvertsTo>(meter);
        log::info("Converting 50cm to m = {}", convert(50, centimeter, meter));
        log::info("Converting 100m to m = {}", convert(100, meter, centimeter));
        meter.removeRelation<ConvertsTo>(centimeter);
        log::info("Converting 50m to m = {}", convert(50,meter, centimeter));
    }
    void Init() override {
        //Initialize component's friendly name
        World::component<TestComponent>().name("TestGame::TestComponent");
        World::component<ComponentA>().name("TestGame::ComponentA");
        World::component<ComponentB>().name("TestGame::ComponentB");
        World::component<ComponentC>().name("TestGame::ComponentC");
        World::component<ComponentD>().name("TestGame::ComponentD");

        Scene s = World::createScene("TestGameScene");
        Entity e = s.entity();
        e.name("Entity");
        e.addComponent<TestComponent>("First");

        TestQuery(s);
        TestRelations(s);

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
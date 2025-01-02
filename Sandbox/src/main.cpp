#include "RoxEngine/core/Logger.hpp"
#include "RoxEngine/input/Input.hpp"
#include <glm/glm.hpp>
#include <string>
#include <RoxEngine/roxengine.hpp> //better for lsps
import roxengine;
using namespace RoxEngine;

struct TestGame final : public Game {
    Ref<VertexArray> va;
    Ref<Shader> shader;
    Ref<Material> material;
    Ref<Framebuffer> fb;
    DefaultRenderer renderer;
    struct TestComponent
    {
        TestComponent(const std::string& s) { a = s; log::info("TestComponent::TestComponent(const std::string&)");}
        TestComponent(const TestComponent& other) { a = std::move(other.a); log::info("TestComponent::TestComponent(TestComponent&)"); }
        ~TestComponent() { log::info("TestComponent::~TestComponent()"); }
        TestComponent& operator=(const TestComponent& other) { a = std::move(other.a);  log::info("TestComponent::operator=(const TestComponent&)"); return *this; }
	    std::string a = "<OOPS>";
    };
     
    void Init() override {
        Scene s;
        Entity e = s.CreateEntity();
    	log::info(e.HasComponent<TestComponent>());
        e.AddComponent<TestComponent>("First");
        log::info(e.HasComponent<TestComponent>());
        log::info(e.GetComponent<TestComponent>().a);
        e.GetComponent<TestComponent>().a = "Second!";
    	log::info(e.GetComponent<TestComponent>().a);
        e.RemoveComponent<TestComponent>();
        log::info(e.AddComponent<TestComponent>({ "Third!" }).a);
        e.RemoveComponent<TestComponent>();
        log::info(e.HasComponent<TestComponent>());
		
        Mesh mesh;
        mesh.position= {
            {-0.5f, -0.5f,  0.5f},  // bottom-left
			{ 0.5f, -0.5f,  0.5f},  // bottom-right
			{ 0.5f,  0.5f,  0.5f},  // top-right
			{-0.5f,  0.5f,  0.5f},  // top-left

			// Back face
			{-0.5f, -0.5f, -0.5f}, // bottom-left
			{ 0.5f, -0.5f, -0.5f}, // bottom-right
			{ 0.5f,  0.5f, -0.5f}, // top-right
			{-0.5f,  0.5f, -0.5f}, // top-left

			// Left face
			{-0.5f, -0.5f, -0.5f}, // bottom-back
			{-0.5f, -0.5f,  0.5f}, // bottom-front
			{-0.5f,  0.5f,  0.5f}, // top-front
			{-0.5f,  0.5f, -0.5f}, // top-back

			// Right face
			{ 0.5f, -0.5f, -0.5f},  // bottom-back
			{ 0.5f, -0.5f,  0.5f},  // bottom-front
			{ 0.5f,  0.5f,  0.5f},  // top-front
			{ 0.5f,  0.5f, -0.5f},  // top-back

			// Top face
			{-0.5f,  0.5f,  0.5f},  // front-left
			{ 0.5f,  0.5f,  0.5f},  // front-right
			{ 0.5f,  0.5f, -0.5f},  // back-right
			{-0.5f,  0.5f, -0.5f},  // back-left

			// Bottom face
			{-0.5f, -0.5f,  0.5f}, // front-left
			{ 0.5f, -0.5f,  0.5f}, // front-right
			{ 0.5f, -0.5f, -0.5f}, // back-right
			{-0.5f, -0.5f, -0.5f}  // back-left
        };
    	mesh.indices = {
            // Front face
		    0, 1, 2,  0, 2, 3,
		    // Back face
		    4, 5, 6,  4, 6, 7,
		    // Left face
		    8, 9, 10,  8, 10, 11,
		    // Right face
		    12, 13, 14,  12, 14, 15,
		    // Top face
		    16, 17, 18,  16, 18, 19,
		    // Bottom face
		    20, 21, 22,  20, 22, 23
        };

    	renderer.SetCamera({}, {});
        renderer.DrawMesh(mesh);
        fb = Framebuffer::Create(800, 800, {FramebufferColorTexFormat::RGB32}, FramebufferDepthTexFormat::D24UNS8U);
        shader = Shader::Create("res://shaders/basic.slang");
        material = Material::Create(shader, Material::EntryPointInfo{ "basic_vmain", "basic_fmain" });
        renderer.debugMaterial = Material::Create(shader, Material::EntryPointInfo{ "basic_vmain", "basic_fmain" });
        glm::mat4 a = { 1.0f };
        glm::vec3 color = { 0,0,1 };
    	renderer.debugMaterial->GetUbo("Uniforms")->Set("matrix", &a[0][0], sizeof(glm::mat4));
        renderer.debugMaterial->GetUbo("Uniforms")->Set("color", &color.r, sizeof(color));

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
        auto lastKeyPressed = Input::GetLastKeyPressed();
        if (auto* v = std::get_if<KeyCode>(&lastKeyPressed)) {
            KeyCode keycode = *v;
            log::info("Last key pressed was {}", size_t(keycode));
            Input::ResetLastKeyPressed();
        } else if (auto* v = std::get_if<MouseButton>(&lastKeyPressed)) {
            MouseButton mousebutton = *v;
            log::info("Last key pressed was mouse button {}", size_t(mousebutton));
            Input::ResetLastKeyPressed();
        }
    }
    void Render() override {
        GraphicsContext::ClearScreen();
        GraphicsContext::UseMaterial(material);
        renderer.DebugMenu();
        renderer.Render();
    }
};

Scope<Game> CreateGame() {
    return CreateScope<TestGame>();
}

//todo: move constructor to the engine project
int main(int, char**) {
    return Engine::Get()->Run(CreateGame());
}